#ifndef LIB_MILKTEA_TIMERFUTURE_H_
#define LIB_MILKTEA_TIMERFUTURE_H_

#ifdef __cplusplus
#include <milktea/timerunit.h>
namespace MilkTea {

class TimerFuture final {
  friend class TimerTask;
  friend class TimerWorker;
 public:
  enum class State {
    SCHEDULED,
    EXECUTED,
    CANCELLED,
    NORMAL,
    EXCEPTIONAL
  };
  TimerFuture(TimerFuture &&another) : TimerFuture() {
    std::swap(self_, another.self_);
  }
  ~TimerFuture() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_panic(MilkTea_TimerFuture_Destroy(self_));
    self_ = nullptr;
  }
  bool Cancel() {
    bool success = false;
    MilkTea_panic(MilkTea_TimerFuture_Cancel(self_, &success));
    return success;
  }
  State GetState() {
    MilkTea_TimerFuture_State_t state = MilkTea_TimerFuture_State_EXCEPTIONAL;
    MilkTea_panic(MilkTea_TimerFuture_GetState(self_, &state));
    return FromRawType(state);
  }
  TimerUnit::time_point_type GetTime() {
    int64_t time = 0;
    MilkTea_panic(MilkTea_TimerFuture_GetTime(self_, &time));
    return TimerUnit::time_point_type(TimerUnit::duration_type(time));
  }
 private:
  static State FromRawType(MilkTea_TimerFuture_State_t state) {
    switch (state) {
      case MilkTea_TimerFuture_State_SCHEDULED: return State::SCHEDULED;
      case MilkTea_TimerFuture_State_EXECUTED: return State::EXECUTED;
      case MilkTea_TimerFuture_State_CANCELLED: return State::CANCELLED;
      case MilkTea_TimerFuture_State_NORMAL: return State::NORMAL;
      case MilkTea_TimerFuture_State_EXCEPTIONAL: return State::EXCEPTIONAL;
      default: MilkTea_assert("State assert");
    }
  }
  TimerFuture(MilkTea_TimerFuture_t *self = nullptr) : self_(self) {}
  MilkTea_TimerFuture_t *self_;
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMoveAssign(TimerFuture)
  static constexpr char TAG[] = "MilkTea#TimerFuture";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERFUTURE_H_
