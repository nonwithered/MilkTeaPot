#include <soymilk/common.h>

#include "player.h"

namespace {

constexpr char TAG[] = "SoyMilk";

SoyMilk::PlayerImpl &soymilk_cast(SoyMilk_Player_t *self) {
  return *reinterpret_cast<SoyMilk::PlayerImpl *>(self);
}

SoyMilk_Player_t *soymilk_cast(SoyMilk::PlayerImpl &self) {
  return reinterpret_cast<SoyMilk_Player_t *>(&self);
}

} // namespace

MilkTea_extern(SoyMilk_Player_Create, (SoyMilk_Player_t **self, SoyMilk_Player_Renderer_t renderer, TeaPot_Executor_t executor, TeaPot_TimerWorker_Weak_t *timer), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(renderer.interface_);
  MilkTea_nonnull(executor.invoke_);
  MilkTea_nonnull(timer);
  *self = soymilk_cast(*new SoyMilk::PlayerImpl(renderer, TeaPot::Executor::FromRawType(executor), timer));
})

MilkTea_extern(SoyMilk_Player_Destroy, (SoyMilk_Player_t *self), {
  MilkTea_nonnull(self);
  delete &soymilk_cast(self);
})

MilkTea_extern(SoyMilk_Player_GetState, (SoyMilk_Player_t *self, SoyMilk_Player_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  *state = SoyMilk::Player::ToRawType(soymilk_cast(self).state());
})

MilkTea_extern(SoyMilk_Player_Prepare, (SoyMilk_Player_t *self, const MilkPowder_Midi_t *midi), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(midi);
  soymilk_cast(self).Prepare(midi);
})

MilkTea_extern(SoyMilk_Player_Start, (SoyMilk_Player_t *self), {
  MilkTea_nonnull(self);
  soymilk_cast(self).Start();
})

MilkTea_extern(SoyMilk_Player_Pause, (SoyMilk_Player_t *self), {
  MilkTea_nonnull(self);
  soymilk_cast(self).Pause();
})

MilkTea_extern(SoyMilk_Player_Seek, (SoyMilk_Player_t *self, int64_t time), {
  MilkTea_nonnull(self);
  soymilk_cast(self).Seek(TeaPot::TimerUnit::duration_type(time));
})

MilkTea_extern(SoyMilk_Player_Resume, (SoyMilk_Player_t *self), {
  MilkTea_nonnull(self);
  soymilk_cast(self).Resume();
})

MilkTea_extern(SoyMilk_Player_Stop, (SoyMilk_Player_t *self), {
  MilkTea_nonnull(self);
  soymilk_cast(self).Stop();
})

MilkTea_extern(SoyMilk_Player_Reset, (SoyMilk_Player_t *self), {
  MilkTea_nonnull(self);
  soymilk_cast(self).Reset();
})
