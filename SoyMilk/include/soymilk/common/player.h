#ifndef LIB_SOYMILK_WRAPPER_PLAYER_H_
#define LIB_SOYMILK_WRAPPER_PLAYER_H_

#include <soymilk/common.h>

namespace SoyMilk {

namespace Player {

constexpr char TAG[] = "SoyMilk::Player";

enum class State {
  INIT,
  PREPARING,
  PREPARED,
  STARTED,
  PLAYING,
  PAUSED,
  SUSPEND,
  SEEKING,
};

inline
State FromRawType(SoyMilk_Player_State_t state) {
  switch (state) {
    case SoyMilk_Player_State_INIT: return State::INIT;
    case SoyMilk_Player_State_PREPARING: return State::PREPARING;
    case SoyMilk_Player_State_PREPARED: return State::PREPARED;
    case SoyMilk_Player_State_STARTED: return State::STARTED;
    case SoyMilk_Player_State_PLAYING: return State::PLAYING;
    case SoyMilk_Player_State_PAUSED: return State::PAUSED;
    case SoyMilk_Player_State_SUSPEND: return State::SUSPEND;
    case SoyMilk_Player_State_SEEKING: return State::SEEKING;
    default: MilkTea_assert("State FromRawType assert");
  }
}

inline
SoyMilk_Player_State_t ToRawType(State state) {
  switch (state) {
    case State::INIT: return SoyMilk_Player_State_INIT;
    case State::PREPARING: return SoyMilk_Player_State_PREPARING;
    case State::PREPARED: return SoyMilk_Player_State_PREPARED;
    case State::STARTED: return SoyMilk_Player_State_STARTED;
    case State::PLAYING: return SoyMilk_Player_State_PLAYING;
    case State::PAUSED: return SoyMilk_Player_State_PAUSED;
    case State::SUSPEND: return SoyMilk_Player_State_SUSPEND;
    case State::SEEKING: return SoyMilk_Player_State_SEEKING;
    default: MilkTea_assert("State ToRawType assert");
  }
}

} // namespace Player
} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_WRAPPER_PLAYER_H_
