#ifndef LIB_YOGURT_CORE_RECORD_H_
#define LIB_YOGURT_CORE_RECORD_H_

#include <milktea.h>
#include <soybean.h>
#include <milkpowder.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Yogurt_Recorder_t;

#ifndef __cplusplus
typedef struct Yogurt_Recorder_t Yogurt_Recorder_t;
#endif

enum Yogurt_Recorder_State_t {
  Yogurt_Recorder_State_PAUSE,
  Yogurt_Recorder_State_RESUME,
  Yogurt_Recorder_State_CLOSED
};

#ifndef __cplusplus
typedef enum Yogurt_Recorder_State_t Yogurt_Recorder_State_t;
#endif

MilkTea_api
Yogurt_Recorder_Create(Yogurt_Recorder_t **obj, uint16_t division);

MilkTea_api
Yogurt_Recorder_Destroy(Yogurt_Recorder_t *obj);

MilkTea_api
Yogurt_Recorder_GetState(Yogurt_Recorder_t *obj, Yogurt_Recorder_State_t *state);

MilkTea_api
Yogurt_Recorder_Resume(Yogurt_Recorder_t *obj);

MilkTea_api
Yogurt_Recorder_Pause(Yogurt_Recorder_t *obj);

MilkTea_api
Yogurt_Recorder_Take(Yogurt_Recorder_t *obj, MilkPowder_Midi_t **midi);

MilkTea_api
Yogurt_Recorder_Factory_Create(Yogurt_Recorder_t *obj, uint32_t tempo, SoyBean_Factory_t *factory);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_YOGURT_CORE_RECORD_H_
