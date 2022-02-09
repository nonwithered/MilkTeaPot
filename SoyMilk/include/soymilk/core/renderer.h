#ifndef LIB_SOYMILK_CORE_RENDERER_H_
#define LIB_SOYMILK_CORE_RENDERER_H_

#include <milktea.h>
#include <milkpowder.h>

#include <soymilk/core/frame.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyMilk_Player_Renderer_Interface_t {
  void (MilkTea_call *Deleter)(void *obj);
  void (MilkTea_call *OnFrame)(void *obj, const SoyMilk_FrameBuffer_t *fbo);
  void (MilkTea_call *OnPrepare)(void *obj, int64_t time);
  void (MilkTea_call *OnStart)(void *obj);
  void (MilkTea_call *OnResume)(void *obj);
  void (MilkTea_call *OnSeekBegin)(void *obj);
  void (MilkTea_call *OnSeekEnd)(void *obj, int64_t time);
  void (MilkTea_call *OnPause)(void *obj, int64_t time);
  void (MilkTea_call *OnStop)(void *obj);
  void (MilkTea_call *OnReset)(void *obj);
  void (MilkTea_call *OnComplete)(void *obj);
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Renderer_Interface_t SoyMilk_Player_Renderer_Interface_t;
#endif

struct SoyMilk_Player_Renderer_t {
  void *obj_;
  const SoyMilk_Player_Renderer_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Renderer_t SoyMilk_Player_Renderer_t;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_RENDERER_H_
