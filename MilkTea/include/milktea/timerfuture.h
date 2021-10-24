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
 public:
  using future_type = std::shared_ptr<TimerFuture>;
  using clock_type = std::chrono::system_clock;
  using duration_type = std::chrono::milliseconds;
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;
  static future_type Create(time_point_type time) {
    return future_type(new TimerFuture(time));
  }
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
  TimerFuture(time_point_type time)
  : state_(State::SCHEDULED),
    time_(time) {}
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  std::atomic<State> state_;
  const time_point_type time_;
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMove(TimerFuture)
  static constexpr char TAG[] = "MilkTea#TimerFuture";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERFUTURE_H_
