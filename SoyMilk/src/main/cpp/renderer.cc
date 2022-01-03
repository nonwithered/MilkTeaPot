#include <soymilk/common.h>

namespace {

constexpr char TAG[] = "SoyMilk";

using duration_type = TeaPot::TimerUnit::duration_type;

SoyMilk::BaseRenderer &BaseRenderer_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyMilk::BaseRenderer *>(self);
}

void MilkTea_call SoyMilk_BaseRenderer_Deleter(void *self) {
  std::move(BaseRenderer_cast(self)).Destroy();
}
void MilkTea_call SoyMilk_BaseRenderer_OnFrame(void *self, const SoyMilk::Codec::FrameBufferWrapper::raw_type *fbo) {
  BaseRenderer_cast(self).OnFrame(fbo);
}
void MilkTea_call SoyMilk_BaseRenderer_OnPrepare(void *self, int64_t time) {
  BaseRenderer_cast(self).OnPrepare(duration_type(time));
}
void MilkTea_call SoyMilk_BaseRenderer_OnStart(void *self) {
  BaseRenderer_cast(self).OnStart();
}
void MilkTea_call SoyMilk_BaseRenderer_OnPause(void *self, int64_t time) {
  BaseRenderer_cast(self).OnPause(duration_type(time));
}
void MilkTea_call SoyMilk_BaseRenderer_OnSeekBegin(void *self) {
  BaseRenderer_cast(self).OnSeekBegin();
}
void MilkTea_call SoyMilk_BaseRenderer_OnSeekEnd(void *self, int64_t time) {
  BaseRenderer_cast(self).OnSeekEnd(duration_type(time));
}
void MilkTea_call SoyMilk_BaseRenderer_OnResume(void *self) {
  BaseRenderer_cast(self).OnResume();
}
void MilkTea_call SoyMilk_BaseRenderer_OnStop(void *self) {
  BaseRenderer_cast(self).OnStop();
}
void MilkTea_call SoyMilk_BaseRenderer_OnReset(void *self) {
  BaseRenderer_cast(self).OnReset();
}
void MilkTea_call SoyMilk_BaseRenderer_OnComplete(void *self) {
  BaseRenderer_cast(self).OnComplete();
}

} // namespace

namespace SoyMilk {

const SoyMilk_Player_Renderer_Interface_t &BaseRenderer::Interface() {
  static const SoyMilk_Player_Renderer_Interface_t interface_{
    .Deleter = SoyMilk_BaseRenderer_Deleter,
    .OnFrame = SoyMilk_BaseRenderer_OnFrame,
    .OnPrepare = SoyMilk_BaseRenderer_OnPrepare,
    .OnStart = SoyMilk_BaseRenderer_OnStart,
    .OnPause = SoyMilk_BaseRenderer_OnPause,
    .OnSeekBegin = SoyMilk_BaseRenderer_OnSeekBegin,
    .OnSeekEnd = SoyMilk_BaseRenderer_OnSeekEnd,
    .OnResume = SoyMilk_BaseRenderer_OnResume,
    .OnStop = SoyMilk_BaseRenderer_OnStop,
    .OnReset = SoyMilk_BaseRenderer_OnReset,
    .OnComplete = SoyMilk_BaseRenderer_OnComplete,
  };
  return interface_;
}

} // namespace SoyMilk
