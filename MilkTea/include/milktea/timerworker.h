#ifndef LIB_MILKTEA_TIMERWORKER_H_
#define LIB_MILKTEA_TIMERWORKER_H_

#ifdef __cplusplus
#include <milktea/defer.h>
#include <milktea/timertask.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <tuple>
namespace MilkTea {

class TimerWorker final {
 public:
  using worker_type = std::shared_ptr<TimerWorker>;
  using task_type = TimerTask::task_type;
  using future_type = TimerTask::future_type;
  using clock_type = TimerTask::clock_type;
  using duration_type = TimerTask::duration_type;
  using time_point_type = TimerTask::time_point_type;
  using action_type = TimerTask::action_type;
  enum class State {
    INIT,
    RUNNING,
    SHUTDOWN,
    STOP,
    TIDYING,
    TERMINATED
  };
  explicit TimerWorker(std::function<bool(std::exception *)> on_terminate = [](std::exception *) -> bool { return false; })
  : state_(State::INIT),
    on_terminate_(on_terminate) {}
  ~TimerWorker() {
    State state = state_;
    if (state_ == State::INIT) {
      return;
    }
    if (state_ != State::TERMINATED) {
      MilkTea_logE("dtor -- state %s", StateName(state));
      Shutdown();
      AwaitTermination();
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
  std::vector<task_type> ShutdownNow() {
    std::vector<task_type> vec;
    ChangeState(State::RUNNING, State::STOP, [this, &vec]() -> void {
      while (!tasks_.empty()) {
        vec.push_back(tasks_.top());
        tasks_.pop();
      }
      tasks_cond_.notify_one();
    });
    return vec;
  }
  future_type Post(action_type f, duration_type delay = duration_type::zero()) {
    if (delay < duration_type::zero()) {
      delay = duration_type::zero();
    }
    time_point_type target = CurrentTimePoint() + delay;
    future_type future = TimerFuture::Create(target);
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING) {
      future->Cancel();
      return future;
    }
    bool wake = tasks_.empty() || *tasks_.top() - target > duration_type::zero();
    tasks_.push(TimerTask::Create(future, f));
    if (wake) {
      tasks_cond_.notify_one();
    }
    return future;
  }
 private:
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
      }
      if (!tasks_.empty()) {
        if (state_ == State::STOP) {
          MilkTea_logE("Take assert State::STOP");
          MilkTea_assert("Take assert State::STOP");
        }
        duration_type delta = *tasks_.top() - CurrentTimePoint();
        if (delta > duration_type::zero()) {
          tasks_cond_.wait_for(guard, delta);
          continue;
        } else {
          task_type task = std::move(tasks_.top());
          tasks_.pop();
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
  State state_;
  std::priority_queue<task_type, std::vector<task_type>, std::greater<task_type>> tasks_;
  std::recursive_mutex lock_;
  std::condition_variable_any state_cond_;
  std::condition_variable_any tasks_cond_;
  std::function<bool(std::exception *)> on_terminate_;
  MilkTea_NonCopy(TimerWorker)
  MilkTea_NonMove(TimerWorker)
  static constexpr char TAG[] = "MilkTea#TimerWorker";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERWORKER_H_
