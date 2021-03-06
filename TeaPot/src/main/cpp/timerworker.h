#ifndef TEAPOT_TIMERWORKER_H_
#define TEAPOT_TIMERWORKER_H_

#include <queue>
#include <condition_variable>

#include "timertask.h"
#include "timermanager.h"

namespace TeaPot {

class TimerWorkerImpl final : public std::enable_shared_from_this<TimerWorkerImpl> {
  static constexpr char TAG[] = "TeaPot::TimerWorkerImpl";
  using State = TimerWorker::State;
  using duration_type = TimerUnit::duration_type;
  using time_point_type = TimerUnit::time_point_type;
  using clock_type = TimerUnit::clock_type;
 public:
  static worker_type Make(std::function<bool(MilkTea::Exception::Type, std::string_view)> termination) {
    auto worker = worker_type(new TimerWorkerImpl(termination));
    worker->binder_.Bind(worker);
    return worker;
  }
  static void Close(worker_type worker) {
    worker->ChangeStateOr(State::TERMINATED, State::CLOSED, [](State state) -> void {
      if (state == State::INIT) {
        MilkTea_logW("Close before start");
        return;
      }
      MilkTea_throwf(LogicError, "Close need TERMINATED but now %s", StateName(state));
    });
    worker->binder_.Unbind(std::move(worker));
  }
  ~TimerWorkerImpl() {
    State state = state_;
    if (state == State::INIT) {
      MilkTea_logW("dtor before start");
      return;
    }
    if (state != State::CLOSED) {
      auto [success, tasks] = ShutdownNow();
      MilkTea_logW("dtor -- state %s tasks %" PRIu32, StateName(state), static_cast<uint32_t>(tasks.size()));
    }
  }
  void Start() {
    ChangeStateOr(State::INIT, State::RUNNING, [](State state) -> void {
      MilkTea_throwf(LogicError, "start need INIT but now %s", StateName(state));
    });
    Run();
  }
  State state() const {
    return state_;
  }
  void AwaitTermination() {
    std::unique_lock guard(lock_);
    State state = state_;
    while (state != State::TERMINATED && state != State::CLOSED) {
      state_cond_.wait(guard);
    }
  }
  bool AwaitTermination(duration_type delay) {
    time_point_type target = CurrentTimePoint() + delay;
    std::unique_lock guard(lock_);
    MilkTea_loop {
      State state = state_;
      if (state == State::TERMINATED || state == State::CLOSED) {
        return true;
      }
      duration_type delta = target - CurrentTimePoint();
      if (delta > duration_type::zero()) {
        state_cond_.wait_for(guard, delta);
      } else {
        return false;
      }
    }
  }
  bool Shutdown() {
    return ChangeStateAnd(State::RUNNING, State::SHUTDOWN, [this]() -> void {
      tasks_cond_.notify_one();
    });
  }
  std::tuple<bool, std::vector<task_type>> ShutdownNow() {
    bool b = false;
    std::vector<task_type> vec;
    ChangeStateAnd(State::RUNNING, State::STOP, [this, &b, &vec]() -> void {
      b = true;
      while (!tasks_.empty()) {
        vec.push_back(Poll());
      }
      tasks_cond_.notify_one();
    });
    return std::make_tuple(b, std::move(vec));
  }
  future_type Post(duration_type delay, action_type f) {
    if (delay < duration_type::zero()) {
      delay = duration_type::zero();
    }
    time_point_type target = CurrentTimePoint() + delay;
    future_type future = TimerFutureImpl::Make(target);
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING && state_ != State::INIT) {
      future->Cancel();
      return future;
    }
    future->SetCancel(GetCancel());
    auto task_top = Peek();
    bool wake = task_top == nullptr || *task_top - target > duration_type::zero();
    Offer(TimerTaskImpl::Make(future, f));
    if (wake) {
      tasks_cond_.notify_one();
    }
    return future;
  }
 private:
  explicit TimerWorkerImpl(std::function<bool(MilkTea::Exception::Type, std::string_view)> termination)
  : state_(State::INIT),
    termination_(termination),
    binder_() {}
  void Run() {
    auto type = MilkTea::Exception::Suppress([this]() {
      MilkTea_loop {
        auto [b, f] = Take();
        if (!b) {
          break;
        }
        f();
      }
    });
    TryTerminate(type, MilkTea::Exception::What());
  }
  bool TryTerminate(MilkTea::Exception::Type type, std::string_view what) {
    return ChangeState(State::TIDYING, [](State state) -> bool {
      switch (state) {
        case State::INIT:
          MilkTea_assert("TryTerminate assert State::INIT");
        case State::TIDYING:
        case State::TERMINATED:
        case State::CLOSED:
          return false;
        case State::RUNNING:
        case State::SHUTDOWN:
        case State::STOP:
          return true;
        default:
          MilkTea_assert("TryTerminate assert default");
      }
    }) && OnTerminate(type, what);
  }
  bool OnTerminate(MilkTea::Exception::Type type, std::string_view what) {
    MilkTea_defer({
      if (!ChangeStateAnd(State::TIDYING, State::TERMINATED, [this]() -> void {
        state_cond_.notify_all();
      })) {
        MilkTea_assert("OnTerminate assert");
      }
    });
    return termination_(type, what);
  }
  std::tuple<bool, action_type> Take() {
    std::unique_lock guard(lock_);
    MilkTea_loop {
      switch (state_) {
        case State::INIT:
          MilkTea_assert("Take assert State::INIT");
        case State::TIDYING:
          MilkTea_assert("Take assert State::TIDYING");
        case State::TERMINATED:
          MilkTea_assert("Take assert State::TERMINATED");
        case State::CLOSED:
          MilkTea_assert("Take assert State::CLOSED");
        default:
          break;
      }
      auto task_top = Peek();
      if (task_top != nullptr) {
        if (state_ == State::STOP) {
          MilkTea_assert("Take assert State::STOP");
        }
        while (task_top->future()->state() == TimerFuture::State::CANCELLED) {
          Poll();
        }
        duration_type delta = *task_top - CurrentTimePoint();
        if (delta > duration_type::zero()) {
          tasks_cond_.wait_for(guard, delta);
          continue;
        } else {
          return std::make_tuple(true, [task = std::shared_ptr<task_type::element_type>(Poll().release())]() -> void {
            task->Run();
          });
        }
      } else {
        if (state_ == State::RUNNING) {
          tasks_cond_.wait(guard);
          continue;
        } else {
          return std::make_tuple(false, []() -> void {});
        }
      }
    }
  }
  bool ChangeState(State target, std::function<bool(State)> predicate = [](State) -> bool { return true; }, action_type action = []() -> void {}, std::function<void(State)> otherwise = [](State) -> void {}) {
    std::lock_guard guard(lock_);
    if (!predicate(state_)) {
      otherwise(state_);
      return false;
    }
    state_ = target;
    action();
    return true;
  }
  bool ChangeState(State expect, State target, action_type action = []() -> void {}, std::function<void(State)> otherwise = [](State) -> void {}) {
    return ChangeState(target, [expect](State state) -> bool { return state == expect; }, action, otherwise);
  }
  bool ChangeStateAnd(State expect, State target, action_type action) {
    return ChangeState(expect, target, action);
  }
  bool ChangeStateOr(State expect, State target, std::function<void(State)> otherwise) {
    return ChangeState(expect, target, []() -> void {}, otherwise);
  }
  void Offer(task_type &&task) {
    tasks_.push(task.release());
  }
  task_type Poll() {
    task_raw task = tasks_.top();
    tasks_.pop();
    return task_type(task);
  }
  task_raw Peek() {
    if (tasks_.empty()) {
      return nullptr;
    }
    return tasks_.top();
  }
  void OnCancel(future_type future) {
    std::unique_lock guard(lock_);
    auto task_top = Peek();
    if (task_top != nullptr && task_top->future() == future) {
      tasks_cond_.notify_one();
    }
  }
  std::function<void(future_type)> GetCancel() {
    return [binder = binder_](future_type future) -> void {
      auto binder_ = binder;
      binder_.WithGuard([future](worker_type obj) -> void {
        obj->OnCancel(future);
      });
    };
  }
  static time_point_type CurrentTimePoint() {
    return TimerUnit::Now();
  }
  static const char *StateName(State state) {
    return TimerWorker::StateName(state).data();
  }
  struct greater_task_raw {
    bool operator()(const task_raw lhs, const task_raw rhs) const {
      return *lhs > *rhs;
    }
  };
  State state_;
  std::priority_queue<task_raw, std::vector<task_raw>, greater_task_raw> tasks_;
  std::recursive_mutex lock_;
  std::condition_variable_any state_cond_;
  std::condition_variable_any tasks_cond_;
  std::function<bool(MilkTea::Exception::Type, std::string_view)> termination_;
  worker_binder binder_;
  MilkTea_NonCopy(TimerWorkerImpl)
  MilkTea_NonMove(TimerWorkerImpl)
};

} // namespace TeaPot

#endif // ifndef TEAPOT_TIMERWORKER_H_
