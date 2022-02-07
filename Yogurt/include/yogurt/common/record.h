#ifndef LIB_YOGURT_COMMON_RECORD_H_
#define LIB_YOGURT_COMMON_RECORD_H_

#include <milktea.h>

#include <yogurt/core.h>

namespace Yogurt {

enum class RecorderState {
  PAUSE,
  RESUME,
  CLOSED,
};

inline
const char *StateName(RecorderState state) {
  static constexpr char TAG[] = "Yogurt::RecorderState";
  switch (state) {
    case RecorderState::PAUSE: return "PAUSE";
    case RecorderState::RESUME: return "RESUME";
    case RecorderState::CLOSED: return "CLOSED";
    default: MilkTea_assert("StateName");
  }
}

inline
RecorderState FromRawType(Yogurt_Recorder_State_t state) {
  static constexpr char TAG[] = "Yogurt::RecorderState";
  switch (state) {
    case Yogurt_Recorder_State_PAUSE: return RecorderState::PAUSE;
    case Yogurt_Recorder_State_RESUME: return RecorderState::RESUME;
    case Yogurt_Recorder_State_CLOSED: return RecorderState::CLOSED;
    default: MilkTea_assert("FromRawType assert");
  }
}

inline
Yogurt_Recorder_State_t ToRawType(RecorderState state) {
  static constexpr char TAG[] = "Yogurt::RecorderState";
  switch (state) {
    case RecorderState::PAUSE: return Yogurt_Recorder_State_PAUSE;
    case RecorderState::RESUME: return Yogurt_Recorder_State_RESUME;
    case RecorderState::CLOSED: return Yogurt_Recorder_State_CLOSED;
    default: MilkTea_assert("ToRawType assert");
  }
}

} // namespace Yogurt

#endif // ifndef LIB_YOGURT_COMMON_RECORD_H_
