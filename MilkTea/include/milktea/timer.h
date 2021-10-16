#ifndef LIB_MILKTEA_TIMER_H_
#define LIB_MILKTEA_TIMER_H_

#ifdef __cplusplus
#include <milktea/exception.h>
#include <milktea/defer.h>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <chrono>
#include <atomic>
#include <exception>
namespace MilkTea {

class Timer final {
 private:
  using clock_type = std::chrono::system_clock;
  using duration_type = std::chrono::milliseconds;
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;
  using action_type = std::function<void()>;
  using task_type = std::tuple<time_point_type, action_type>;
  static bool OnTerminateDefault(std::exception *e) {
    return false;
  }
  struct greater_type {
    bool operator()(const task_type& lhs, const task_type& rhs) const {
      return TargetTimePoint(lhs) > TargetTimePoint(rhs);
    }
  };
 public:
  enum class State {
    INIT,
    RUNNING,
    SHUTDOWN,
    STOP,
    TIDYING,
    TERMINATED
  };
  Timer(std::function<bool(std::exception *)> onTerminate = OnTerminateDefault)
  : onTerminate_(onTerminate),
    state_(State::INIT) {}
  State state() const {
    return state_;
  }
  bool Start(std::function<void(action_type)> callback) {
    {
      std::lock_guard guard(lock_);
      if (state_ != State::INIT) {
        return false;
      }
      state_ = State::RUNNING;
    }
    callback(std::bind(Run, this));
    return true;
  }
  bool AwaitTermination(duration_type duration) {
    time_point_type target = CurrentTimePoint() + duration;
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
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING) {
      return false;
    }
    state_ = State::SHUTDOWN;
    tasks_cond_.notify_one();
    return true;
  }
  std::vector<task_type> ShutdownNow() {
    std::vector<task_type> vec;
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING) {
      return vec;
    }
    state_ = State::STOP;
    tasks_cond_.notify_one();
    while (!tasks_.empty()) {
      vec.push_back(tasks_.top());
      tasks_.pop();
    }
    return vec;
  }
  time_point_type Submit(action_type f, duration_type delay = duration_type::zero()) {
    if (delay < duration_type::zero()) {
      delay = duration_type::zero();
    }
    time_point_type target = CurrentTimePoint() + delay;
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING) {
      return time_point_type();
    }
    bool wake = tasks_.empty() || TargetTimePoint(tasks_.top()) > target;
    tasks_.push(std::make_tuple(target, f));
    if (wake) {
      tasks_cond_.notify_one();
    }
    return target;
  }
 private:
  void Run() {
    try {
      while (true) {
        auto [b, f] = Poll();
        f();
        if (!b) {
          break;
        }
      }
    } catch (std::exception &e) {
      bool b = TryTerminate(&e);
      if (!b) {
        throw e;
      }
    }
  }
  bool TryTerminate(std::exception *e) {
    {
      std::lock_guard guard(lock_);
      if (state_ == State::TERMINATED) {
        return false;
      }
      state_ = State::TIDYING;
    }
    return OnTerminate(e);
  }
  bool OnTerminate(std::exception *e) {
    Defer defer([this]() -> void {
      std::lock_guard guard(lock_);
      state_ = State::TERMINATED;
      state_cond_.notify_all();
    });
    return onTerminate_(e);
  }
  std::tuple<bool, action_type> Poll() {
    std::unique_lock guard(lock_);
    while (true) {
      switch (state_) {
        case State::INIT:
          MilkTea_LogPrint(ERROR, TAG, "Poll assert State::INIT");
          MilkTea_throw(Assertion, "INIT");
        case State::TIDYING:
          MilkTea_LogPrint(ERROR, TAG, "Poll assert State::TIDYING");
          MilkTea_throw(Assertion, "TIDYING");
        case State::TERMINATED:
          MilkTea_LogPrint(ERROR, TAG, "Poll assert State::TERMINATED");
          MilkTea_throw(Assertion, "TERMINATED");
        default:
          break;
      }
      if (!tasks_.empty()) {
        if (state_ == State::STOP) {
          MilkTea_LogPrint(ERROR, TAG, "Poll assert State::STOP");
          MilkTea_throw(Assertion, "STOP");
        }
        duration_type delta = TargetTimePoint(tasks_.top()) - CurrentTimePoint();
        if (delta > duration_type::zero()) {
          tasks_cond_.wait_for(guard, delta);
          continue;
        } else {
          std::tuple<bool, action_type> action = std::make_tuple(true, ActionOf(tasks_.top()));
          tasks_.pop();
          return action;
        }
      } else {
        if (state_ == State::RUNNING) {
          tasks_cond_.wait(guard);
          continue;
        } else {
          return std::make_tuple(false, std::bind(TryTerminate, this, nullptr));
        }
      }
    }
  }
  static time_point_type CurrentTimePoint() {
    return std::chrono::time_point_cast<duration_type>(clock_type::now());
  }
  static time_point_type TargetTimePoint(const task_type &task) {
    return std::get<0>(task);
  }
  static action_type ActionOf(const task_type &task) {
    return std::get<1>(task);
  }
  std::function<bool(std::exception *)> onTerminate_;
  State state_;
  std::priority_queue<task_type, std::vector<task_type>, greater_type> tasks_;
  std::mutex lock_;
  std::condition_variable state_cond_;
  std::condition_variable tasks_cond_;
  static constexpr char TAG[] = "timer";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMER_H_
