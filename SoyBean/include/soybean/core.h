#ifndef LIB_SOYBEAN_CORE_H_
#define LIB_SOYBEAN_CORE_H_

#include <milktea.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyBean_Handle_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Deleter)(void *obj);
  MilkTea_Exception_t (MilkTea_call *NoteOff)(void *obj, uint8_t channel, uint8_t note, uint8_t pressure);
  MilkTea_Exception_t (MilkTea_call *NoteOn)(void *obj, uint8_t channel, uint8_t note, uint8_t pressure);
  MilkTea_Exception_t (MilkTea_call *AfterTouch)(void *obj, uint8_t channel, uint8_t note, uint8_t pressure);
  MilkTea_Exception_t (MilkTea_call *ControlChange)(void *obj, uint8_t channel, uint8_t control, uint8_t argument);
  MilkTea_Exception_t (MilkTea_call *ProgramChange)(void *obj, uint8_t channel, uint8_t program);
  MilkTea_Exception_t (MilkTea_call *ChannelPressure)(void *obj, uint8_t channel, uint8_t pressure);
  MilkTea_Exception_t (MilkTea_call *PitchBend)(void *obj, uint8_t channel, uint8_t low, uint8_t height);
};

#ifndef __cplusplus
typedef struct SoyBean_Handle_Interface_t SoyBean_Handle_Interface_t;
#endif

struct SoyBean_Handle_t {
  void *obj_;
  const SoyBean_Handle_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyBean_Handle_t SoyBean_Handle_t;
#endif

struct SoyBean_Factory_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Deleter)(void *obj);
  MilkTea_Exception_t (MilkTea_call *Create)(void *obj, SoyBean_Handle_t *handle);
};

#ifndef __cplusplus
typedef struct SoyBean_Factory_Interface_t SoyBean_Factory_Interface_t;
#endif

struct SoyBean_Factory_t {
  void *obj_;
  const SoyBean_Factory_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyBean_Factory_t SoyBean_Factory_t;
#endif

MilkTea_api
SoyBean_Handle_Create(SoyBean_Handle_t *obj, SoyBean_Factory_t factory);

MilkTea_api
SoyBean_Handle_Destroy(SoyBean_Handle_t obj);

MilkTea_api
SoyBean_Handle_NoteOff(SoyBean_Handle_t obj, uint8_t channel, uint8_t note, uint8_t pressure);

MilkTea_api
SoyBean_Handle_NoteOn(SoyBean_Handle_t obj, uint8_t channel, uint8_t note, uint8_t pressure);

MilkTea_api
SoyBean_Handle_AfterTouch(SoyBean_Handle_t obj, uint8_t channel, uint8_t note, uint8_t pressure);

MilkTea_api
SoyBean_Handle_ControlChange(SoyBean_Handle_t obj, uint8_t channel, uint8_t control, uint8_t argument);

MilkTea_api
SoyBean_Handle_ProgramChange(SoyBean_Handle_t obj, uint8_t channel, uint8_t program);

MilkTea_api
SoyBean_Handle_ChannelPressure(SoyBean_Handle_t obj, uint8_t channel, uint8_t pressure);

MilkTea_api
SoyBean_Handle_PitchBend(SoyBean_Handle_t obj, uint8_t channel, uint8_t low, uint8_t height);

MilkTea_api
SoyBean_Factory_Destroy(SoyBean_Factory_t obj);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_CORE_H_
