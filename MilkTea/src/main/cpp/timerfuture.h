#ifndef MILKTEA_TIMERFUTURE_H_
#define MILKTEA_TIMERFUTURE_H_

#include <milktea/util.h>

#include <chrono>
#include <atomic>
#include <memory>

namespace MilkTea {

class TimerFutureImpl final {
  friend class TimerTaskImpl;
 public:
  using future_type = std::shared_ptr<TimerFutureImpl>;
  using clock_type = std::chrono::system_clock;
  using duration_type = std::chrono::milliseconds;
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;
  static future_type Create(time_point_type time) {
    return future_type(new TimerFutureImpl(time));
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
  TimerFutureImpl(time_point_type time)
  : state_(State::SCHEDULED),
    time_(time) {}
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  std::atomic<State> state_;
  const time_point_type time_;
  static constexpr char TAG[] = "timerfuture";
  MilkTea_NonCopy(TimerFutureImpl)
  MilkTea_NonMove(TimerFutureImpl)
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERFUTURE_H_
