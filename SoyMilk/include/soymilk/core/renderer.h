#ifndef LIB_SOYMILK_CORE_RENDERER_H_
#define LIB_SOYMILK_CORE_RENDERER_H_

#include <milktea.h>
#include <milkpowder.h>

#include <soymilk/core/frame.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyMilk_Player_Renderer_Interface_t {
  void (MilkTea_call *Deleter)(void *self);
  void (MilkTea_call *OnFrame)(void *self, const SoyMilk_FrameBuffer_t *fbo);
  void (MilkTea_call *OnPrepare)(void *self, int64_t time);
  void (MilkTea_call *OnStart)(void *self);
  void (MilkTea_call *OnPause)(void *self, int64_t time);
  void (MilkTea_call *OnSeekBegin)(void *self);
  void (MilkTea_call *OnSeekEnd)(void *self, int64_t time);
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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_RENDERER_H_
