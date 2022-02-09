#ifndef LIB_SOYMILK_CORE_PLAYER_H_
#define LIB_SOYMILK_CORE_PLAYER_H_

#include <milkpowder.h>
#include <teapot.h>

#include <soymilk/core/renderer.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SoyMilk_Player_State_t {
  SoyMilk_Player_State_INIT,
  SoyMilk_Player_State_PREPARING,
  SoyMilk_Player_State_PREPARED,
  SoyMilk_Player_State_STARTED,
  SoyMilk_Player_State_RESUMED,
  SoyMilk_Player_State_PLAYING,
  SoyMilk_Player_State_SUSPEND,
  SoyMilk_Player_State_SEEKING,
  SoyMilk_Player_State_PAUSED,
  SoyMilk_Player_State_STOPPED,
  SoyMilk_Player_State_RESET
};

#ifndef __cplusplus
typedef enum SoyMilk_Player_State_t SoyMilk_Player_State_t;
#endif

struct SoyMilk_Player_t;

#ifndef __cplusplus
typedef struct SoyMilk_Player_t SoyMilk_Player_t;
#endif

MilkTea_api
SoyMilk_Player_Create(SoyMilk_Player_t **obj, SoyMilk_Player_Renderer_t renderer, TeaPot_Executor_t executor, TeaPot_TimerWorker_Weak_t *timer);

MilkTea_api
SoyMilk_Player_Destroy(SoyMilk_Player_t *obj);

MilkTea_api
SoyMilk_Player_GetState(SoyMilk_Player_t *obj, SoyMilk_Player_State_t *state);

MilkTea_api
SoyMilk_Player_Prepare(SoyMilk_Player_t *obj, const MilkPowder_Midi_t *midi);

MilkTea_api
SoyMilk_Player_Start(SoyMilk_Player_t *obj);

MilkTea_api
SoyMilk_Player_Pause(SoyMilk_Player_t *obj);

MilkTea_api
SoyMilk_Player_Seek(SoyMilk_Player_t *obj, int64_t time);

MilkTea_api
SoyMilk_Player_Resume(SoyMilk_Player_t *obj);

MilkTea_api
SoyMilk_Player_Stop(SoyMilk_Player_t *obj);

MilkTea_api
SoyMilk_Player_Reset(SoyMilk_Player_t *obj);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_PLAYER_H_
