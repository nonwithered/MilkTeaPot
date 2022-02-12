#include <soymilk/common.h>

namespace {

constexpr char TAG[] = "SoyMilk";

SoyMilk::BaseRenderer &BaseRenderer_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<SoyMilk::BaseRenderer *>(obj);
}

void MilkTea_call SoyMilk_BaseRenderer_Deleter(void *obj) {
  std::move(BaseRenderer_cast(obj)).Destroy();
}
void MilkTea_call SoyMilk_BaseRenderer_OnFrame(void *obj, const SoyMilk::FrameBufferWrapper::raw_type *fbo) {
  SoyMilk::FrameBufferWrapper fbo_ = const_cast<SoyMilk::FrameBufferWrapper::raw_type *>(fbo);
  MilkTea::Defer defer([&fbo_]() {
    fbo_.release();
  });
  BaseRenderer_cast(obj).OnFrame(fbo_);
}
void MilkTea_call SoyMilk_BaseRenderer_OnPrepare(void *obj, int64_t time) {
  BaseRenderer_cast(obj).OnPrepare(SoyMilk::tempo_type(time));
}
void MilkTea_call SoyMilk_BaseRenderer_OnStart(void *obj) {
  BaseRenderer_cast(obj).OnStart();
}
void MilkTea_call SoyMilk_BaseRenderer_OnResume(void *obj) {
  BaseRenderer_cast(obj).OnResume();
}
void MilkTea_call SoyMilk_BaseRenderer_OnSeekBegin(void *obj) {
  BaseRenderer_cast(obj).OnSeekBegin();
}
void MilkTea_call SoyMilk_BaseRenderer_OnSeekEnd(void *obj, int64_t time) {
  BaseRenderer_cast(obj).OnSeekEnd(SoyMilk::tempo_type(time));
}
void MilkTea_call SoyMilk_BaseRenderer_OnPause(void *obj, int64_t time) {
  BaseRenderer_cast(obj).OnPause(SoyMilk::tempo_type(time));
}
void MilkTea_call SoyMilk_BaseRenderer_OnStop(void *obj) {
  BaseRenderer_cast(obj).OnStop();
}
void MilkTea_call SoyMilk_BaseRenderer_OnReset(void *obj) {
  BaseRenderer_cast(obj).OnReset();
}
void MilkTea_call SoyMilk_BaseRenderer_OnComplete(void *obj) {
  BaseRenderer_cast(obj).OnComplete();
}

} // namespace

namespace SoyMilk {

const SoyMilk_Player_Renderer_Interface_t &BaseRenderer::Interface() {
  static const SoyMilk_Player_Renderer_Interface_t interface_{
    .Deleter = SoyMilk_BaseRenderer_Deleter,
    .OnFrame = SoyMilk_BaseRenderer_OnFrame,
    .OnPrepare = SoyMilk_BaseRenderer_OnPrepare,
    .OnStart = SoyMilk_BaseRenderer_OnStart,
    .OnResume = SoyMilk_BaseRenderer_OnResume,
    .OnSeekBegin = SoyMilk_BaseRenderer_OnSeekBegin,
    .OnSeekEnd = SoyMilk_BaseRenderer_OnSeekEnd,
    .OnPause = SoyMilk_BaseRenderer_OnPause,
    .OnStop = SoyMilk_BaseRenderer_OnStop,
    .OnReset = SoyMilk_BaseRenderer_OnReset,
    .OnComplete = SoyMilk_BaseRenderer_OnComplete,
  };
  return interface_;
}

} // namespace SoyMilk
