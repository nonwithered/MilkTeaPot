#ifndef MILKTEA_TIMERWORKER_H_
#define MILKTEA_TIMERWORKER_H_

#include <queue>
#include <condition_variable>

#include <milktea.h>

#include "timertask.h"
#include "timermanager.h"

namespace MilkTea {

#ifdef MilkTea_TimerWorker_typedef
#undef MilkTea_TimerWorker_typedef
#endif

#define MilkTea_TimerWorker_typedef \
  MilkTea_TimerFuture_typedef \
  MilkTea_TimerTask_typedef \
  MilkTea_TimerManager_typedef

class TimerWorkerImpl final : public std::enable_shared_from_this<TimerWorkerImpl> {
  MilkTea_TimerWorker_typedef
 public:
  enum class State {
    INIT,
    RUNNING,
    SHUTDOWN,
    STOP,
    TIDYING,
    TERMINATED
  };
  static worker_type Make(std::function<bool(std::exception *)> on_terminate) {
    return worker_type(new TimerWorkerImpl(on_terminate));
  }
  void Attach(manager_type manager) {
    holder_ = worker_holder(manager, manager->Register(weak_from_this()));
  }
  void Detach(worker_type &&self) {
    State state = state_;
    if (state != State::TERMINATED) {
      MilkTea_throwf(LogicError, "Delete need TERMINATED but now %s", StateName(state));
    }
    holder_.Close(std::forward<worker_type>(self));
  }
  ~TimerWorkerImpl() {
    State state = state_;
    if (state_ == State::INIT) {
      MilkTea_logW("dtor before start");
      return;
    }
    if (state_ != State::TERMINATED) {
      auto [success, tasks] = ShutdownNow();
      MilkTea_logW("dtor -- state %s tasks %" PRIu32, StateName(state), static_cast<uint32_t>(tasks.size()));
    }
  }
  void Start() {
    ChangeStateOr(State::INIT, State::RUNNING, [this]() -> void {
      MilkTea_throwf(LogicError, "start need INIT but now %s", StateName(state_));
    });
    Run();
  }
  State state() const {
    return state_;
  }
  void AwaitTermination() {
    std::unique_lock guard(lock_);
    while (state_ != State::TERMINATED) {
      state_cond_.wait(guard);
    }
  }
  bool AwaitTermination(duration_type delay) {
    time_point_type target = CurrentTimePoint() + delay;
    std::unique_lock guard(lock_);
    while (true) {
      if (state_ == State::TERMINATED) {
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
    if (state_ != State::RUNNING) {
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
  explicit TimerWorkerImpl(std::function<bool(std::exception *)> on_terminate)
  : state_(State::INIT),
    on_terminate_(on_terminate),
    holder_() {}
  void Run() {
    try {
      while (true) {
        auto [b, f] = Take();
        if (!b) {
          break;
        }
        f();
      }
      TryTerminate(nullptr);
    } catch (std::exception &e) {
      if (!TryTerminate(&e)) {
        throw e;
      }
    }
  }
  bool TryTerminate(std::exception *e) {
    return ChangeState(State::TIDYING, [](State state) -> bool {
      switch (state) {
        case State::INIT:
          MilkTea_assert("TryTerminate assert State::INIT");
        case State::RUNNING:
          MilkTea_assert("TryTerminate assert State::RUNNING");
        case State::TIDYING:
        case State::TERMINATED:
          return false;
        case State::SHUTDOWN:
        case State::STOP:
          return true;
        default:
          MilkTea_assert("TryTerminate assert default");
      }
    }) && OnTerminate(e);
  }
  bool OnTerminate(std::exception *e) {
    Defer defer([this]() -> void {
      if (!ChangeStateAnd(State::TIDYING, State::TERMINATED, [this]() -> void {
        state_cond_.notify_all();
      })) {
        MilkTea_assert("OnTerminate assert");
      }
    });
    return on_terminate_(e);
  }
  std::tuple<bool, action_type> Take() {
    std::unique_lock guard(lock_);
    while (true) {
      switch (state_) {
        case State::INIT:
          MilkTea_assert("Take assert State::INIT");
        case State::TIDYING:
          MilkTea_assert("Take assert State::TIDYING");
        case State::TERMINATED:
          MilkTea_assert("Take assert State::TERMINATED");
        default:
          break;
      }
      auto task_top = Peek();
      if (task_top != nullptr) {
        if (state_ == State::STOP) {
          MilkTea_assert("Take assert State::STOP");
        }
        while (task_top->future()->state() == TimerFutureImpl::State::CANCELLED) {
          Poll();
        }
        duration_type delta = *task_top - CurrentTimePoint();
        if (delta > duration_type::zero()) {
          tasks_cond_.wait_for(guard, delta);
          continue;
        } else {
          auto task = std::shared_ptr<task_type::element_type>(Poll().release());
          return std::make_tuple(true, [task]() -> void {
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
  bool ChangeState(State target, std::function<bool(State)> predicate = [](State) -> bool { return true; }, action_type action = []() -> void {}, action_type otherwise = []() -> void {}) {
    std::lock_guard guard(lock_);
    if (!predicate(state_)) {
      otherwise();
      return false;
    }
    state_ = target;
    action();
    return true;
  }
  bool ChangeState(State expect, State target, action_type action = []() -> void {}, action_type otherwise = []() -> void {}) {
    return ChangeState(target, [expect](State state) -> bool { return state == expect; }, action, otherwise);
  }
  bool ChangeStateAnd(State expect, State target, action_type action) {
    return ChangeState(expect, target, action);
  }
  bool ChangeStateOr(State expect, State target, action_type otherwise) {
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
  void OnCancel(future_weak future) {
    std::unique_lock guard(lock_);
    auto task_top = Peek();
    if (task_top != nullptr && task_top->future() == future.lock()) {
      tasks_cond_.notify_one();
    }
  }
  std::function<void(future_weak)> GetCancel() {
    worker_holder holder = holder_;
    auto self = weak_from_this();
    return [holder, self](future_weak future) -> void {
      auto holder_ = holder;
      holder_.Obtain([self, future]() -> void {
        auto self_ = self.lock();
        if (self_ == nullptr) {
          MilkTea_logW("Cancel after dtor");
          return;
        }
        self_->OnCancel(future);
      });
    };
  }
  static time_point_type CurrentTimePoint() {
    return std::chrono::time_point_cast<duration_type>(clock_type::now());
  }
  static const char *StateName(State state) {
    switch (state) {
      case State::INIT: return "INIT";
      case State::RUNNING: return "RUNNING";
      case State::SHUTDOWN: return "SHUTDOWN";
      case State::STOP: return "STOP";
      case State::TIDYING: return "TIDYING";
      case State::TERMINATED: return "TERMINATED";
      default: MilkTea_assert("StateName assert");
    }
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
  std::function<bool(std::exception *)> on_terminate_;
  worker_holder holder_;
  MilkTea_NonCopy(TimerWorkerImpl)
  MilkTea_NonMove(TimerWorkerImpl)
  static constexpr char TAG[] = "MilkTea#TimerWorker";
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERWORKER_H_
