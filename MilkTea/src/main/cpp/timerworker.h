#ifndef MILKTEA_TIMERWORKER_H_
#define MILKTEA_TIMERWORKER_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <tuple>

#include <milktea.h>

#include "timertask.h"

namespace MilkTea {

class TimerWorkerImpl final : public std::enable_shared_from_this<TimerWorkerImpl> {
  using future_weak = TimerFutureImpl::future_weak;
  using future_type = TimerFutureImpl::future_type;
  using clock_type = TimerFutureImpl::clock_type;
  using duration_type = TimerFutureImpl::duration_type;
  using time_point_type = TimerFutureImpl::time_point_type;
  using task_raw = TimerTaskImpl::task_raw;
  using task_type = TimerTaskImpl::task_type;
  using action_type = TimerTaskImpl::action_type;
 public:
  using worker_weak = std::weak_ptr<TimerWorkerImpl>;
  using worker_type = std::shared_ptr<TimerWorkerImpl>;
  enum class State {
    INIT,
    RUNNING,
    SHUTDOWN,
    STOP,
    TIDYING,
    TERMINATED
  };
  static worker_type Create(std::function<bool(std::exception *)> on_terminate) {
    return worker_type(new TimerWorkerImpl(on_terminate));
  }
  ~TimerWorkerImpl() {
    State state = state_;
    if (state_ == State::INIT) {
      return;
    }
    if (state_ != State::TERMINATED) {
      auto [success, tasks] = ShutdownNow();
      MilkTea_logE("dtor -- state %s tasks %" PRIu32, StateName(state), static_cast<uint32_t>(tasks.size()));
    }
  }
  bool Start() {
    if (!ChangeState(State::INIT, State::RUNNING)) {
      return false;
    }
    Run();
    return true;
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
    return ChangeState(State::RUNNING, State::SHUTDOWN, [this]() -> void {
      tasks_cond_.notify_one();
    });
  }
  std::tuple<bool, std::vector<task_type>> ShutdownNow() {
    bool b = false;
    std::vector<task_type> vec;
    ChangeState(State::RUNNING, State::STOP, [this, &b, &vec]() -> void {
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
    future_type future = TimerFutureImpl::Create(target);
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING) {
      future->Cancel();
      return future;
    }
    future->SetCancel(GetCancel());
    bool wake = tasks_.empty() || *tasks_.top() - target > duration_type::zero();
    Offer(TimerTaskImpl::Create(future, f));
    if (wake) {
      tasks_cond_.notify_one();
    }
    return future;
  }
 private:
  explicit TimerWorkerImpl(std::function<bool(std::exception *)> on_terminate)
  : state_(State::INIT),
    on_terminate_(on_terminate) {}
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
          MilkTea_logE("TryTerminate assert State::INIT");
          MilkTea_assert("TryTerminate assert State::INIT");
        case State::RUNNING:
          MilkTea_logE("TryTerminate assert State::RUNNING");
          MilkTea_assert("TryTerminate assert State::RUNNING");
        case State::TIDYING:
        case State::TERMINATED:
          return false;
        case State::SHUTDOWN:
        case State::STOP:
          return true;
        default:
          MilkTea_logE("TryTerminate assert default");
          MilkTea_assert("TryTerminate assert default");
      }
    }) && OnTerminate(e);
  }
  bool OnTerminate(std::exception *e) {
    Defer defer([this]() -> void {
      if (!ChangeState(State::TIDYING, State::TERMINATED, [this]() -> void {
        state_cond_.notify_all();
      })) {
        MilkTea_logE("OnTerminate assert");
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
          MilkTea_logE("Take assert State::INIT");
          MilkTea_assert("Take assert State::INIT");
        case State::TIDYING:
          MilkTea_logE("Take assert State::TIDYING");
          MilkTea_assert("Take assert State::TIDYING");
        case State::TERMINATED:
          MilkTea_logE("Take assert State::TERMINATED");
          MilkTea_assert("Take assert State::TERMINATED");
        default:
          break;
      }
      if (!tasks_.empty()) {
        if (state_ == State::STOP) {
          MilkTea_logE("Take assert State::STOP");
          MilkTea_assert("Take assert State::STOP");
        }
        while (tasks_.top()->future()->state() == TimerFutureImpl::State::CANCELLED) {
          Poll();
        }
        duration_type delta = *tasks_.top() - CurrentTimePoint();
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
  void OnCancel(future_weak future) {
    std::unique_lock guard(lock_);
    if (!tasks_.empty() && tasks_.top()->future() == future.lock()) {
      tasks_cond_.notify_one();
    }
  }
  std::function<void(future_weak)> GetCancel() {
    worker_weak weak = weak_from_this();
    return [weak](future_weak future) -> void {
      worker_type worker = weak.lock();
      if (worker) {
        worker->OnCancel(future);
      }
    };
  }
  bool ChangeState(State target, std::function<bool(State)> predicate = [](State) -> bool { return true; }, action_type action = []() -> void {}) {
    std::lock_guard guard(lock_);
    if (!predicate(state_)) {
      return false;
    }
    state_ = target;
    action();
    return true;
  }
  bool ChangeState(State expect, State target, action_type action = []() -> void {}) {
    return ChangeState(target, [expect](State state) -> bool { return state == expect; }, action);
  }
  void Offer(task_type &&task) {
    tasks_.push(task.release());
  }
  task_type Poll() {
    task_raw task = tasks_.top();
    tasks_.pop();
    return task_type(task);
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
  MilkTea_NonCopy(TimerWorkerImpl)
  MilkTea_NonMove(TimerWorkerImpl)
  static constexpr char TAG[] = "MilkTea#TimerWorker";
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERWORKER_H_
