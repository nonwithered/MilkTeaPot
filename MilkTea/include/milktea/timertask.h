#ifndef LIB_MILKTEA_TIMERTASK_H_
#define LIB_MILKTEA_TIMERTASK_H_

#ifdef __cplusplus
#include <milktea/logger.h>
#include <functional>
#include <chrono>
#include <atomic>
#include <exception>
#include <memory>
namespace MilkTea {
class TimerFuture final {
  friend class TimerTask;
 public:
  enum class State {
    SCHEDULED,
    EXECUTED,
    CANCELLED,
    NORMAL,
    EXCEPTIONAL
  };
  TimerFuture() : state_(State::SCHEDULED) {}
  State state() const {
    return state_;
  }
  bool Cancel() {
    return ChangeState(State::SCHEDULED, State::CANCELLED);
  }
 private:
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  std::atomic<State> state_;
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMove(TimerFuture)
};
class TimerTask final {
  friend class Timer;
 private:
  using clock_type = std::chrono::system_clock;
  using duration_type = std::chrono::milliseconds;
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;
  using action_type = std::function<void()>;
  using future_type = std::shared_ptr<TimerFuture>;
 public:
  TimerTask(future_type future, time_point_type time, action_type action) : future_(future), time_(time), action_(action) {}
  time_point_type time() const {
    return time_;
  }
 private:
  void Run() {
    if (!future_->ChangeState(TimerFuture::State::SCHEDULED, TimerFuture::State::EXECUTED)) {
        return;
    }
    try {
      action_();
      future_->ChangeState(TimerFuture::State::EXECUTED, TimerFuture::State::NORMAL);
    } catch (std::exception &e) {
      future_->ChangeState(TimerFuture::State::EXECUTED, TimerFuture::State::EXCEPTIONAL);
      throw e;
    }
  }
  future_type future_;
  const time_point_type time_;
  const action_type action_;
  static constexpr char TAG[] = "timertask";
  MilkTea_NonCopy(TimerTask)
  MilkTea_NonMove(TimerTask)
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERTASK_H_
