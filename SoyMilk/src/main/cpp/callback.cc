#include <soymilk.h>

#define BaseCallback_self \
MilkTea_nonnull(self); \
auto &self_ = *reinterpret_cast<SoyMilk::BaseCallback *>(self);

namespace {

constexpr char TAG[] = "SoyMilk#extern";

void MilkTea_CALL SoyMilk_BaseCallback_OnPlay(void *self, int64_t time, uint16_t ntrk, MilkPowder_Message_t *message) {
  BaseCallback_self
  self_.OnPlay(SoyMilk::BaseCallback::duration_type(time), ntrk, MilkPowder::Message(message));
}
void MilkTea_CALL SoyMilk_BaseCallback_OnPrepare(void *self, int64_t time) {
  BaseCallback_self
  self_.OnPrepare(SoyMilk::BaseCallback::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseCallback_OnStart(void *self) {
  BaseCallback_self
  self_.OnStart();
}
void MilkTea_CALL SoyMilk_BaseCallback_OnPause(void *self, int64_t time) {
  BaseCallback_self
  self_.OnPause(SoyMilk::BaseCallback::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseCallback_OnSeekBegin(void *self, int64_t time) {
  BaseCallback_self
  self_.OnSeekBegin(SoyMilk::BaseCallback::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseCallback_OnSeekEnd(void *self) {
  BaseCallback_self
  self_.OnSeekEnd();
}
void MilkTea_CALL SoyMilk_BaseCallback_OnResume(void *self) {
  BaseCallback_self
  self_.OnResume();
}
void MilkTea_CALL SoyMilk_BaseCallback_OnStop(void *self, int64_t time) {
  BaseCallback_self
  self_.OnStop(SoyMilk::BaseCallback::duration_type(time));
}
void MilkTea_CALL SoyMilk_BaseCallback_OnComplete(void *self) {
  BaseCallback_self
  self_.OnComplete();
}
void MilkTea_CALL SoyMilk_BaseCallback_OnReset(void *self) {
  BaseCallback_self
  self_.OnReset();
}

const SoyMilk_Player_Callback_Interface_t &SoyMilk_BaseCallback_Interface_Instance() {
  static const SoyMilk_Player_Callback_Interface_t interface_{
    .OnPlay = SoyMilk_BaseCallback_OnPlay,
    .OnPrepare = SoyMilk_BaseCallback_OnPrepare,
    .OnStart = SoyMilk_BaseCallback_OnStart,
    .OnPause = SoyMilk_BaseCallback_OnPause,
    .OnSeekBegin = SoyMilk_BaseCallback_OnSeekBegin,
    .OnSeekEnd = SoyMilk_BaseCallback_OnSeekEnd,
    .OnResume = SoyMilk_BaseCallback_OnResume,
    .OnStop = SoyMilk_BaseCallback_OnStop,
    .OnComplete = SoyMilk_BaseCallback_OnComplete,
    .OnReset = SoyMilk_BaseCallback_OnReset,
  };
  return interface_;
}

} // namespace

namespace SoyMilk {

SoyMilk_Player_Callback_t BaseCallback::ToRawCallback() {
  return SoyMilk_Player_Callback_t{
    .self_ = this,
    .interface_ = &SoyMilk_BaseCallback_Interface_Instance(),
  };
}

} // namespace SoyMilk
