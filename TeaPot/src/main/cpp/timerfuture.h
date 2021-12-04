#ifndef TEAPOT_TIMERFUTURE_H_
#define TEAPOT_TIMERFUTURE_H_

#include <chrono>
#include <atomic>
#include <memory>
#include <functional>

#include <teapot/common.h>

namespace TeaPot {

class TimerFutureImpl;
using future_raw = TimerFutureImpl;
using future_weak = std::weak_ptr<future_raw>;
using future_type = std::shared_ptr<future_raw>;

class TimerFutureImpl final : public std::enable_shared_from_this<TimerFutureImpl> {
  friend class TimerTaskImpl;
  friend class TimerWorkerImpl;
  using State = TimerFuture::State;
  using time_point_type = TimerUnit::time_point_type;
 public:
  static future_type Make(time_point_type time) {
    return future_type(new TimerFutureImpl(time));
  }
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
  static constexpr char TAG[] = "TeaPot::TimerFutureImpl";
};

} // namespace TeaPot

#endif // ifndef TEAPOT_TIMERFUTURE_H_
