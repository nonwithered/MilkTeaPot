#include <yogurt/common.h>

#include "factory.h"

namespace {

constexpr char TAG[] = "Yogurt";

} // namespace

std::shared_ptr<Yogurt::RecorderImpl> &yogurt_cast(Yogurt_Recorder_t *self) {
  return *reinterpret_cast<std::shared_ptr<Yogurt::RecorderImpl> *>(self);
}

Yogurt_Recorder_t *yogurt_cast(std::shared_ptr<Yogurt::RecorderImpl> &self) {
  return reinterpret_cast<Yogurt_Recorder_t *>(&self);
}

MilkTea_extern(Yogurt_Recorder_Create, (Yogurt_Recorder_t **self, uint16_t division), {
  MilkTea_nonnull(self);
  *self = yogurt_cast(*new std::shared_ptr<Yogurt::RecorderImpl>(Yogurt::RecorderImpl::Make(division)));
})

MilkTea_extern(Yogurt_Recorder_Destroy, (Yogurt_Recorder_t *self), {
  MilkTea_nonnull(self);
  delete &yogurt_cast(self);
})

MilkTea_extern(Yogurt_Recorder_GetState, (Yogurt_Recorder_t *self, Yogurt_Recorder_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  *state = Yogurt::ToRawType(yogurt_cast(self)->state());
})

MilkTea_extern(Yogurt_Recorder_Factory_Create, (Yogurt_Recorder_t *self, uint32_t tempo, SoyBean_Factory_t *factory), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(factory);
  *factory = Yogurt::FactoryImpl(yogurt_cast(self), tempo).ToRawType();
})

MilkTea_extern(Yogurt_Recorder_Resume, (Yogurt_Recorder_t *self), {
  MilkTea_nonnull(self);
  yogurt_cast(self)->Resume();
})

MilkTea_extern(Yogurt_Recorder_Pause, (Yogurt_Recorder_t *self), {
  MilkTea_nonnull(self);
  yogurt_cast(self)->Pause();
})

MilkTea_extern(Yogurt_Recorder_Take, (Yogurt_Recorder_t *self, MilkPowder_Midi_t **midi), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(midi);
  *midi = yogurt_cast(self)->Take().release();
})
