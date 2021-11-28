#ifndef LIB_TEAPOT_TIMERFUTURE_H_
#define LIB_TEAPOT_TIMERFUTURE_H_

#ifdef __cplusplus

#include <teapot/core.h>

namespace TeaPot {

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
    MilkTea_panic(TeaPot_TimerFuture_Destroy(self_));
    self_ = nullptr;
  }
  bool Cancel() {
    bool success = false;
    MilkTea_panic(TeaPot_TimerFuture_Cancel(self_, &success));
    return success;
  }
  State GetState() {
    TeaPot_TimerFuture_State_t state = TeaPot_TimerFuture_State_EXCEPTIONAL;
    MilkTea_panic(TeaPot_TimerFuture_GetState(self_, &state));
    return FromRawType(state);
  }
  time_point_type GetTime() {
    int64_t time = 0;
    MilkTea_panic(TeaPot_TimerFuture_GetTime(self_, &time));
    return time_point_type(duration_type(time));
  }
  static State FromRawType(TeaPot_TimerFuture_State_t state) {
    switch (state) {
      case TeaPot_TimerFuture_State_SCHEDULED: return State::SCHEDULED;
      case TeaPot_TimerFuture_State_EXECUTED: return State::EXECUTED;
      case TeaPot_TimerFuture_State_CANCELLED: return State::CANCELLED;
      case TeaPot_TimerFuture_State_NORMAL: return State::NORMAL;
      case TeaPot_TimerFuture_State_EXCEPTIONAL: return State::EXCEPTIONAL;
      default: MilkTea_assert("State FromRawType assert");
    }
  }
  static TeaPot_TimerFuture_State_t ToRawType(State state) {
    switch (state) {
      case State::SCHEDULED: return TeaPot_TimerFuture_State_SCHEDULED;
      case State::EXECUTED: return TeaPot_TimerFuture_State_EXECUTED;
      case State::CANCELLED: return TeaPot_TimerFuture_State_CANCELLED;
      case State::NORMAL: return TeaPot_TimerFuture_State_NORMAL;
      case State::EXCEPTIONAL: return TeaPot_TimerFuture_State_EXCEPTIONAL;
      default: MilkTea_assert("State ToRawType assert");
    }
  }
 private:
  TimerFuture(TeaPot_TimerFuture_t *self = nullptr) : self_(self) {}
  TeaPot_TimerFuture_t *self_;
  MilkTea_NonCopy(TimerFuture)
  MilkTea_NonMoveAssign(TimerFuture)
  static constexpr char TAG[] = "TeaPot#TimerFuture";
};

} // namespace TeaPot
#endif // ifdef __cplusplus

#endif // ifndef LIB_TEAPOT_TIMERFUTURE_H_
