#ifndef LIB_MILKTEA_TIMERFUTURE_H_
#define LIB_MILKTEA_TIMERFUTURE_H_

#ifdef __cplusplus
#include <milktea/util.h>
#include <chrono>
#include <atomic>
#include <memory>
namespace MilkTea {
class TimerFuture final {
  friend class TimerTask;
 private:
  using future_type = std::shared_ptr<TimerFuture>;
  using clock_type = std::chrono::system_clock;
  using duration_type = std::chrono::milliseconds;
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;
 public:
  static future_type Create(time_point_type time) {
    return std::make_shared<TimerFuture>(time);
  }
  TimerFuture(time_point_type time)
  : state_(State::SCHEDULED),
    time_(time) {}
  enum class State {
    SCHEDULED,
    EXECUTED,
    CANCELLED,
    NORMAL,
    EXCEPTIONAL
  };
  bool Cancel() {
    return ChangeState(State::SCHEDULED, State::CANCELLED);
  }
  State state() const {
    return state_;
  }
  time_point_type time() const {
    return time_;
  }
 private:
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  std::atomic<State> state_;
  const time_point_type time_;
  static constexpr char TAG[] = "timerfuture";
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMove(TimerFuture)
};
} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERFUTURE_H_
