#ifndef LIB_SOYMILK_CORE_H_
#define LIB_SOYMILK_CORE_H_

#include <milkpowder.h>
#include <teapot.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SoyMilk_Player_State_t {
  SoyMilk_Player_State_INIT,
  SoyMilk_Player_State_PREPARING,
  SoyMilk_Player_State_PREPARED,
  SoyMilk_Player_State_STARTED,
  SoyMilk_Player_State_PLAYING,
  SoyMilk_Player_State_PAUSED,
  SoyMilk_Player_State_SUSPEND,
  SoyMilk_Player_State_SEEKING
};

#ifndef __cplusplus
typedef enum SoyMilk_Player_State_t SoyMilk_Player_State_t;
#endif

struct SoyMilk_FrameItem_t {
  uint16_t ntrk_;
  size_t len_;
  MilkPowder_Message_t *msg_;
};

#ifndef __cplusplus
typedef struct SoyMilk_FrameItem_t SoyMilk_FrameItem_t;
#endif

struct SoyMilk_FrameBuffer_t {
  int64_t time_;
  size_t len_;
  SoyMilk_FrameItem_t *items_;
};

#ifndef __cplusplus
typedef struct SoyMilk_FrameBuffer_t SoyMilk_FrameBuffer_t;
#endif

struct SoyMilk_Player_Renderer_Interface_t {
  void (MilkTea_call *Deleter)(void *self);
  void (MilkTea_call *OnRender)(void *self, const SoyMilk_FrameBuffer_t *fbo);
  void (MilkTea_call *OnPrepare)(void *self, int64_t time);
  void (MilkTea_call *OnStart)(void *self);
  void (MilkTea_call *OnPause)(void *self, int64_t time);
  void (MilkTea_call *OnSeekBegin)(void *self);
  void (MilkTea_call *OnSeekEnd)(void *self);
  void (MilkTea_call *OnResume)(void *self);
  void (MilkTea_call *OnStop)(void *self);
  void (MilkTea_call *OnReset)(void *self);
  void (MilkTea_call *OnComplete)(void *self);
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Renderer_Interface_t SoyMilk_Player_Renderer_Interface_t;
#endif

struct SoyMilk_Player_Renderer_t {
  void *self_;
  const SoyMilk_Player_Renderer_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Renderer_t SoyMilk_Player_Renderer_t;
#endif

struct SoyMilk_Player_t;

#ifndef __cplusplus
typedef struct SoyMilk_Player_t SoyMilk_Player_t;
#endif

MilkTea_api
SoyMilk_Player_Create(SoyMilk_Player_t **self, SoyMilk_Player_Renderer_t renderer, TeaPot_Executor_t executor, TeaPot_TimerWorker_Weak_t *timer);

MilkTea_api
SoyMilk_Player_Destroy(SoyMilk_Player_t *self);

MilkTea_api
SoyMilk_Player_GetState(SoyMilk_Player_t *self, SoyMilk_Player_State_t *state);

MilkTea_api
SoyMilk_Player_Prepare(SoyMilk_Player_t *self, const MilkPowder_Midi_t *midi);

MilkTea_api
SoyMilk_Player_Start(SoyMilk_Player_t *self);

MilkTea_api
SoyMilk_Player_Pause(SoyMilk_Player_t *self);

MilkTea_api
SoyMilk_Player_Seek(SoyMilk_Player_t *self, int64_t time);

MilkTea_api
SoyMilk_Player_Resume(SoyMilk_Player_t *self);

MilkTea_api
SoyMilk_Player_Stop(SoyMilk_Player_t *self);

MilkTea_api
SoyMilk_Player_Reset(SoyMilk_Player_t *self);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_H_
