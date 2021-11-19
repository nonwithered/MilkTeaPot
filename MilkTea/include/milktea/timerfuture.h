#ifndef LIB_MILKTEA_TIMERFUTURE_H_
#define LIB_MILKTEA_TIMERFUTURE_H_

#ifdef __cplusplus
#include <milktea/util.h>
#include <chrono>
#include <atomic>
#include <memory>
#include <functional>
namespace MilkTea {

class TimerFuture final : public std::enable_shared_from_this<TimerFuture> {
  friend class TimerTask;
  friend class TimerWorker;
 public:
  using future_weak = std::weak_ptr<TimerFuture>;
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
  explicit TimerFuture(time_point_type time)
  : state_(State::SCHEDULED),
    time_(time),
    on_cancel_() {}
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  void OnCancel() {
    if (on_cancel_) {
      on_cancel_(weak_from_this());
    }
  }
  void SetCancel(std::function<void(future_weak)> on_cancel) {
    on_cancel_ = on_cancel;
  }
  std::atomic<State> state_;
  const time_point_type time_;
  std::function<void(future_weak)> on_cancel_;
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMove(TimerFuture)
  static constexpr char TAG[] = "MilkTea#TimerFuture";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERFUTURE_H_
