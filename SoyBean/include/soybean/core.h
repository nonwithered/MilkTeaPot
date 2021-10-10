#ifndef LIB_SOYBEAN_CORE_H_
#define LIB_SOYBEAN_CORE_H_

#include <milktea.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyBean_Interface_t {
  MilkTea_Exception_t (*deletor)(void *handle);
  MilkTea_Exception_t (*note_off)(void *handle, uint8_t channel, uint8_t note, uint8_t pressure);
  MilkTea_Exception_t (*note_on)(void *handle, uint8_t channel, uint8_t note, uint8_t pressure);
  MilkTea_Exception_t (*after_touch)(void *handle, uint8_t channel, uint8_t note, uint8_t pressure);
  MilkTea_Exception_t (*control_change)(void *handle, uint8_t channel, uint8_t control, uint8_t argument);
  MilkTea_Exception_t (*program_change)(void *handle, uint8_t channel, uint8_t program);
  MilkTea_Exception_t (*channel_pressure)(void *handle, uint8_t channel, uint8_t pressure);
  MilkTea_Exception_t (*pitch_bend)(void *handle, uint8_t channel, uint8_t low, uint8_t height);
};

#ifndef __cplusplus
typedef struct SoyBean_Interface_t SoyBean_Interface_t;
#endif

struct SoyBean_Handle_t {
  void *handle_;
  SoyBean_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyBean_Handle_t SoyBean_Handle_t;
#endif

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_Create(SoyBean_Handle_t *self, void *factory, MilkTea_Exception_t (*callback)(void *factory, SoyBean_Handle_t *handle));

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_Destroy(SoyBean_Handle_t self);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_NoteOff(SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_NoteOn(SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_AfterTouch(SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_ControlChange(SoyBean_Handle_t self, uint8_t channel, uint8_t control, uint8_t argument);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_ProgramChange(SoyBean_Handle_t self, uint8_t channel, uint8_t program);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_ChannelPressure(SoyBean_Handle_t self, uint8_t channel, uint8_t pressure);

MilkTea_API MilkTea_Exception_t
SoyBean_Handle_PitchBend(SoyBean_Handle_t self, uint8_t channel, uint8_t low, uint8_t height);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_CORE_H_
