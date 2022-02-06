#ifndef LIB_YOGURT_CORE_RECORD_H_
#define LIB_YOGURT_CORE_RECORD_H_

#include <milktea.h>
#include <soybean.h>
#include <milkpowder.h>

struct Yogurt_Recorder_t;

#ifndef __cplusplus
typedef struct Yogurt_Recorder_t Yogurt_Recorder_t;
#endif

MilkTea_api
Yogurt_Recorder_Create(Yogurt_Recorder_t **self, uint16_t division);

MilkTea_api
Yogurt_Recorder_Destroy(Yogurt_Recorder_t *self);

MilkTea_api
Yogurt_Recorder_Factory_Create(Yogurt_Recorder_t *self, uint32_t tempo, SoyBean_Factory_t *factory);

MilkTea_api
Yogurt_Recorder_Resume(Yogurt_Recorder_t *self);

MilkTea_api
Yogurt_Recorder_Pause(Yogurt_Recorder_t *self);

MilkTea_api
Yogurt_Recorder_Take(Yogurt_Recorder_t *self, MilkPowder_Midi_t **midi);

#endif // ifndef LIB_YOGURT_CORE_RECORD_H_
