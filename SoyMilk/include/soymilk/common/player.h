#ifndef LIB_SOYMILK_WRAPPER_PLAYER_H_
#define LIB_SOYMILK_WRAPPER_PLAYER_H_

#include <soymilk/core.h>

namespace SoyMilk {

namespace Player {

constexpr char TAG[] = "SoyMilk::Player";

enum class State {
  INIT,
  PREPARING,
  PREPARED,
  STARTED,
  RESUMED,
  PLAYING,
  SUSPEND,
  SEEKING,
  PAUSED,
  STOPPED,
  RESET,
};

inline
State FromRawType(SoyMilk_Player_State_t state) {
  switch (state) {
    case SoyMilk_Player_State_INIT: return State::INIT;
    case SoyMilk_Player_State_PREPARING: return State::PREPARING;
    case SoyMilk_Player_State_PREPARED: return State::PREPARED;
    case SoyMilk_Player_State_STARTED: return State::STARTED;
    case SoyMilk_Player_State_RESUMED: return State::RESUMED;
    case SoyMilk_Player_State_PLAYING: return State::PLAYING;
    case SoyMilk_Player_State_SUSPEND: return State::SUSPEND;
    case SoyMilk_Player_State_SEEKING: return State::SEEKING;
    case SoyMilk_Player_State_PAUSED: return State::PAUSED;
    case SoyMilk_Player_State_STOPPED: return State::STOPPED;
    case SoyMilk_Player_State_RESET: return State::RESET;
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
    case State::RESUMED: return SoyMilk_Player_State_RESUMED;
    case State::PLAYING: return SoyMilk_Player_State_PLAYING;
    case State::SUSPEND: return SoyMilk_Player_State_SUSPEND;
    case State::SEEKING: return SoyMilk_Player_State_SEEKING;
    case State::PAUSED: return SoyMilk_Player_State_PAUSED;
    case State::STOPPED: return SoyMilk_Player_State_STOPPED;
    case State::RESET: return SoyMilk_Player_State_RESET;
    default: MilkTea_assert("State ToRawType assert");
  }
}

inline
std::string_view StateName(State state) {
  switch (state) {
    case State::INIT: return "INIT";
    case State::PREPARING: return "PREPARING";
    case State::PREPARED: return "PREPARED";
    case State::STARTED: return "STARTED";
    case State::RESUMED: return "RESUMED";
    case State::PLAYING: return "PLAYING";
    case State::SUSPEND: return "SUSPEND";
    case State::SEEKING: return "SEEKING";
    case State::PAUSED: return "PAUSED";
    case State::STOPPED: return "STOPPED";
    case State::RESET: return "RESET";
    default: MilkTea_assert("StateName assert");
  }
}

} // namespace Player
} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_WRAPPER_PLAYER_H_
