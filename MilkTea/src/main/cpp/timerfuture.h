#ifndef MILKTEA_TIMERFUTURE_H_
#define MILKTEA_TIMERFUTURE_H_

#include <chrono>
#include <atomic>
#include <memory>
#include <functional>

#include <milktea.h>

#include "exception.h"

namespace MilkTea {

#ifdef MilkTea_TimerFuture_typedef
#undef MilkTea_TimerFuture_typedef
#endif

#define MilkTea_TimerFuture_typedef \
  using future_raw = MilkTea::TimerFutureImpl; \
  using future_weak = std::weak_ptr<future_raw>; \
  using future_type = std::shared_ptr<future_raw>; \
  using clock_type = std::chrono::system_clock; \
  using duration_type = std::chrono::milliseconds; \
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;

class TimerFutureImpl final : public std::enable_shared_from_this<TimerFutureImpl> {
  friend class TimerTaskImpl;
  friend class TimerWorkerImpl;
  MilkTea_TimerFuture_typedef
 public:
  static future_type Make(time_point_type time) {
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
    bool b = ChangeState(State::SCHEDULED, State::CANCELLED);
    if (b) {
      OnCancel();
    }
    return b;
  }
  State state() const {
    return state_;
  }
  time_point_type time() const {
    return time_;
  }
 private:
  explicit TimerFutureImpl(time_point_type time)
  : state_(State::SCHEDULED),
    time_(time),
    on_cancel_() {}
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  void OnCancel() {
    if (on_cancel_) {
      on_cancel_(shared_from_this());
    }
  }
  void SetCancel(std::function<void(future_type)> on_cancel) {
    on_cancel_ = on_cancel;
  }
  std::atomic<State> state_;
  const time_point_type time_;
  std::function<void(future_type)> on_cancel_;
  MilkTea_NonCopy(TimerFutureImpl)
  MilkTea_NonMove(TimerFutureImpl)
  static constexpr char TAG[] = "MilkTea#TimerFuture";
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERFUTURE_H_
