#ifndef LIB_TEAPOT_COMMON_TIMERFUTURE_H_
#define LIB_TEAPOT_COMMON_TIMERFUTURE_H_

#include <teapot/core.h>

namespace TeaPot {

namespace TimerFuture {

constexpr char TAG[] = "TeaPot::TimerFuture";

enum class State {
  SCHEDULED,
  EXECUTED,
  CANCELLED,
  NORMAL,
  EXCEPTIONAL,
};

inline
State FromRawType(TeaPot_TimerFuture_State_t state) {
  switch (state) {
    case TeaPot_TimerFuture_State_SCHEDULED: return State::SCHEDULED;
    case TeaPot_TimerFuture_State_EXECUTED: return State::EXECUTED;
    case TeaPot_TimerFuture_State_CANCELLED: return State::CANCELLED;
    case TeaPot_TimerFuture_State_NORMAL: return State::NORMAL;
    case TeaPot_TimerFuture_State_EXCEPTIONAL: return State::EXCEPTIONAL;
    default: MilkTea_assert("State FromRawType assert");
  }
}

inline
TeaPot_TimerFuture_State_t ToRawType(State state) {
  switch (state) {
    case State::SCHEDULED: return TeaPot_TimerFuture_State_SCHEDULED;
    case State::EXECUTED: return TeaPot_TimerFuture_State_EXECUTED;
    case State::CANCELLED: return TeaPot_TimerFuture_State_CANCELLED;
    case State::NORMAL: return TeaPot_TimerFuture_State_NORMAL;
    case State::EXCEPTIONAL: return TeaPot_TimerFuture_State_EXCEPTIONAL;
    default: MilkTea_assert("State ToRawType assert");
  }
}

} // namespace TimerFuture
} // namespace TeaPot

#endif // define LIB_TEAPOT_COMMON_TIMERFUTURE_H_
