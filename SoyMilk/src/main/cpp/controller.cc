#include <soymilk.h>

namespace {

constexpr char TAG[] = "SoyMilk#extern";

SoyMilk::BaseController &BaseController_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyMilk::BaseController *>(self);
}

void MilkTea_CALL SoyMilk_BaseController_Deleter(void *self) {
  BaseController_cast(self).Deleter();
}

void MilkTea_CALL SoyMilk_BaseController_OnSubmit(void *self, MilkTea_ClosureToken_t obj, void (MilkTea_CALL *submit)(void *obj)) {
  auto callback = MilkTea::ClosureToken<void()>::FromRawType(obj, submit);
  BaseController_cast(self).OnSubmit([callback]() -> void {
    callback();
  });
}
void MilkTea_CALL SoyMilk_BaseController_OnPlay(void *self, int64_t time, uint16_t ntrk, MilkPowder_Message_t *message) {
  BaseController_cast(self).OnPlay(SoyMilk::BaseController::duration_type(time), ntrk, MilkPowder::Message(message));
}
void MilkTea_CALL SoyMilk_BaseController_OnPrepare(void *self, int64_t time) {
  BaseController_cast(self).OnPrepare(SoyMilk::BaseController::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseController_OnStart(void *self) {
  BaseController_cast(self).OnStart();
}
void MilkTea_CALL SoyMilk_BaseController_OnPause(void *self, int64_t time) {
  BaseController_cast(self).OnPause(SoyMilk::BaseController::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseController_OnSeekBegin(void *self, int64_t time) {
  BaseController_cast(self).OnSeekBegin(SoyMilk::BaseController::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseController_OnSeekEnd(void *self) {
  BaseController_cast(self).OnSeekEnd();
}
void MilkTea_CALL SoyMilk_BaseController_OnResume(void *self) {
  BaseController_cast(self).OnResume();
}
void MilkTea_CALL SoyMilk_BaseController_OnStop(void *self, int64_t time) {
  BaseController_cast(self).OnStop(SoyMilk::BaseController::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseController_OnComplete(void *self) {
  BaseController_cast(self).OnComplete();
}
void MilkTea_CALL SoyMilk_BaseController_OnReset(void *self) {
  BaseController_cast(self).OnReset();
}

const SoyMilk_Player_Controller_Interface_t &SoyMilk_BaseController_Interface_Instance() {
  static const SoyMilk_Player_Controller_Interface_t interface_{
    .Deleter = SoyMilk_BaseController_Deleter,
    .OnSubmit = SoyMilk_BaseController_OnSubmit,
    .OnPlay = SoyMilk_BaseController_OnPlay,
    .OnPrepare = SoyMilk_BaseController_OnPrepare,
    .OnStart = SoyMilk_BaseController_OnStart,
    .OnPause = SoyMilk_BaseController_OnPause,
    .OnSeekBegin = SoyMilk_BaseController_OnSeekBegin,
    .OnSeekEnd = SoyMilk_BaseController_OnSeekEnd,
    .OnResume = SoyMilk_BaseController_OnResume,
    .OnStop = SoyMilk_BaseController_OnStop,
    .OnComplete = SoyMilk_BaseController_OnComplete,
    .OnReset = SoyMilk_BaseController_OnReset,
  };
  return interface_;
}

} // namespace

namespace SoyMilk {

SoyMilk_Player_Controller_t BaseController::ToRawType() {
  return SoyMilk_Player_Controller_t{
    .self_ = this,
    .interface_ = &SoyMilk_BaseController_Interface_Instance(),
  };
}

} // namespace SoyMilk
