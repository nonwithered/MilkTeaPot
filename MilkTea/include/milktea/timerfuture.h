#ifndef LIB_MILKTEA_TIMERFUTURE_H_
#define LIB_MILKTEA_TIMERFUTURE_H_

#ifdef __cplusplus
#include <milktea/util.h>
#include <atomic>
#include <memory>
namespace MilkTea {
class TimerFuture final {
  friend class TimerTask;
 private:
  using future_type = std::shared_ptr<TimerFuture>;
 public:
  static future_type Create() {
    return std::move(std::make_shared<TimerFuture>());
  }
  TimerFuture() : state_(State::SCHEDULED) {}
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
 private:
  bool ChangeState(State expect, State target) {
    return state_.compare_exchange_strong(expect, target);
  }
  std::atomic<State> state_;
  static constexpr char TAG[] = "timerfuture";
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMove(TimerFuture)
};
} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERFUTURE_H_
