#include <soymilk/common.h>

#include "player.h"

namespace {

constexpr char TAG[] = "SoyMilk";

std::shared_ptr<SoyMilk::PlayerImpl> &soymilk_cast(SoyMilk_Player_t *obj) {
  return *reinterpret_cast<std::shared_ptr<SoyMilk::PlayerImpl> *>(obj);
}

SoyMilk_Player_t *soymilk_cast(std::shared_ptr<SoyMilk::PlayerImpl> &obj) {
  return reinterpret_cast<SoyMilk_Player_t *>(&obj);
}

} // namespace

MilkTea_extern(SoyMilk_Player_Create, (SoyMilk_Player_t **obj, SoyMilk_Player_Renderer_t renderer, TeaPot_Executor_t executor, TeaPot_TimerWorker_Weak_t *timer), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(renderer.interface_);
  MilkTea_nonnull(executor.invoke_);
  MilkTea_nonnull(timer);
  auto player = std::make_shared<SoyMilk::PlayerImpl>(renderer, TeaPot::Executor::FromRawType(executor), timer);
  player->Init();
  *obj = soymilk_cast(*new std::shared_ptr<SoyMilk::PlayerImpl>(std::move(player)));
})

MilkTea_extern(SoyMilk_Player_Destroy, (SoyMilk_Player_t *obj), {
  MilkTea_nonnull(obj);
  delete &soymilk_cast(obj);
})

MilkTea_extern(SoyMilk_Player_GetState, (SoyMilk_Player_t *obj, SoyMilk_Player_State_t *state), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(state);
  *state = SoyMilk::Player::ToRawType(soymilk_cast(obj)->state());
})

MilkTea_extern(SoyMilk_Player_Prepare, (SoyMilk_Player_t *obj, MilkPowder_Midi_t *midi[], size_t count), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(midi);
  soymilk_cast(obj)->Prepare(std::vector<MilkPowder::MidiMutableWrapper>(midi, midi + count));
})

MilkTea_extern(SoyMilk_Player_Start, (SoyMilk_Player_t *obj), {
  MilkTea_nonnull(obj);
  soymilk_cast(obj)->Start();
})

MilkTea_extern(SoyMilk_Player_Pause, (SoyMilk_Player_t *obj), {
  MilkTea_nonnull(obj);
  soymilk_cast(obj)->Pause();
})

MilkTea_extern(SoyMilk_Player_Seek, (SoyMilk_Player_t *obj, int64_t time), {
  MilkTea_nonnull(obj);
  soymilk_cast(obj)->Seek(SoyMilk::tempo_type(time));
})

MilkTea_extern(SoyMilk_Player_Resume, (SoyMilk_Player_t *obj), {
  MilkTea_nonnull(obj);
  soymilk_cast(obj)->Resume();
})

MilkTea_extern(SoyMilk_Player_Stop, (SoyMilk_Player_t *obj), {
  MilkTea_nonnull(obj);
  soymilk_cast(obj)->Stop();
})

MilkTea_extern(SoyMilk_Player_Reset, (SoyMilk_Player_t *obj), {
  MilkTea_nonnull(obj);
  soymilk_cast(obj)->Reset();
})
