#ifndef LIB_TEAPOT_COMMON_TIMERWORKER_H_
#define LIB_TEAPOT_COMMON_TIMERWORKER_H_

#include <teapot/core.h>

namespace TeaPot {

namespace TimerWorker {

constexpr char TAG[] = "TeaPot::TimerWorker";

enum class State {
  INIT,
  RUNNING,
  SHUTDOWN,
  STOP,
  TIDYING,
  TERMINATED,
  CLOSED,
};

inline
State FromRawType(TeaPot_TimerWorker_State_t state) {
  switch (state) {
    case TeaPot_TimerWorker_State_INIT: return State::INIT;
    case TeaPot_TimerWorker_State_RUNNING: return State::RUNNING;
    case TeaPot_TimerWorker_State_SHUTDOWN: return State::SHUTDOWN;
    case TeaPot_TimerWorker_State_STOP: return State::STOP;
    case TeaPot_TimerWorker_State_TIDYING: return State::TIDYING;
    case TeaPot_TimerWorker_State_TERMINATED: return State::TERMINATED;
    case TeaPot_TimerWorker_State_CLOSED: return State::CLOSED;
    default: MilkTea_assert("State FromRawType assert");
  }
}

inline
TeaPot_TimerWorker_State_t ToRawType(State state) {
  switch (state) {
    case State::INIT: return TeaPot_TimerWorker_State_INIT;
    case State::RUNNING: return TeaPot_TimerWorker_State_RUNNING;
    case State::SHUTDOWN: return TeaPot_TimerWorker_State_SHUTDOWN;
    case State::STOP: return TeaPot_TimerWorker_State_STOP;
    case State::TIDYING: return TeaPot_TimerWorker_State_TIDYING;
    case State::TERMINATED: return TeaPot_TimerWorker_State_TERMINATED;
    default: MilkTea_assert("State ToRawType assert");
  }
}

} // namespace TimerWorker
} // namespace TeaPot

#endif // define LIB_TEAPOT_COMMON_TIMERWORKER_H_
