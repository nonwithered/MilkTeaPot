#ifndef LIB_SOYBEAN_CORE_H_
#define LIB_SOYBEAN_CORE_H_

#include <milktea.h>

#define SoyBean_API MilkTea_API

#ifdef __cplusplus
extern "C" {
#endif

enum SoyBean_Err_t {
  SoyBean_Err_Nil,
  SoyBean_Err_Unknown,
  SoyBean_Err_Assertion,
  SoyBean_Err_NullPointer,
  SoyBean_Err_Unsupported,
  SoyBean_Err_InvalidParam,
  SoyBean_Err_LogicError
};

#ifndef __cplusplus
typedef enum SoyBean_Err_t SoyBean_Err_t;
#endif

inline const char *SobBean_Err_Name(SoyBean_Err_t type) {
  switch (type) {
    case SoyBean_Err_Nil: return "Nil";
    case SoyBean_Err_Assertion: return "Assertion";
    case SoyBean_Err_NullPointer: return "NullPointer";
    case SoyBean_Err_Unsupported: return "Unsupported";
    case SoyBean_Err_InvalidParam: return "InvalidParam";
    case SoyBean_Err_LogicError: return "LogicError";
    default: return "Unknown";
  }
}

struct SobBean_Interface_t {
  SoyBean_Err_t (*note_off)(void *handle, uint8_t channel, uint8_t note, uint8_t pressure);
  SoyBean_Err_t (*note_on)(void *handle, uint8_t channel, uint8_t note, uint8_t pressure);
  SoyBean_Err_t (*after_touch)(void *handle, uint8_t channel, uint8_t note, uint8_t pressure);
  SoyBean_Err_t (*control_change)(void *handle, uint8_t channel, uint8_t control, uint8_t argument);
  SoyBean_Err_t (*program_change)(void *handle, uint8_t channel, uint8_t program);
  SoyBean_Err_t (*channel_pressure)(void *handle, uint8_t channel, uint8_t pressure);
  SoyBean_Err_t (*pitch_bend)(void *handle, uint8_t channel, uint8_t low, uint8_t height);
};

#ifndef __cplusplus
typedef struct SobBean_Interface_t SobBean_Interface_t;
#endif

struct SoyBean_Handle_t {
  void *handle_;
  SobBean_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyBean_Handle_t SoyBean_Handle_t;
#endif

SoyBean_API SoyBean_Err_t
SoyBean_Handle_Create(SoyBean_Handle_t **self, void *factory, SoyBean_Err_t (*callback)(void *factory, SoyBean_Handle_t **handle));

SoyBean_API SoyBean_Err_t
SoyBean_Handle_Destroy(SoyBean_Handle_t *self, void *cleaner, SoyBean_Err_t (*callback)(void *cleaner, SoyBean_Handle_t *handle));

SoyBean_API SoyBean_Err_t
SoyBean_Handle_NoteOff(const SoyBean_Handle_t *self, uint8_t channel, uint8_t note, uint8_t pressure);

SoyBean_API SoyBean_Err_t
SoyBean_Handle_NoteOn(const SoyBean_Handle_t *self, uint8_t channel, uint8_t note, uint8_t pressure);

SoyBean_API SoyBean_Err_t
SoyBean_Handle_AfterTouch(const SoyBean_Handle_t *self, uint8_t channel, uint8_t note, uint8_t pressure);

SoyBean_API SoyBean_Err_t
SoyBean_Handle_ControlChange(const SoyBean_Handle_t *self, uint8_t channel, uint8_t control, uint8_t argument);

SoyBean_API SoyBean_Err_t
SoyBean_Handle_ProgramChange(const SoyBean_Handle_t *self, uint8_t channel, uint8_t program);

SoyBean_API SoyBean_Err_t
SoyBean_Handle_ChannelPressure(const SoyBean_Handle_t *self, uint8_t channel, uint8_t pressure);

SoyBean_API SoyBean_Err_t
SoyBean_Handle_PitchBend(const SoyBean_Handle_t *self, uint8_t channel, uint8_t low, uint8_t height);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_CORE_H_
