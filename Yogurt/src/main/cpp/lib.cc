#include <yogurt/common.h>

#include "factory.h"

namespace {

constexpr char TAG[] = "Yogurt";

} // namespace

std::shared_ptr<Yogurt::RecorderImpl> &yogurt_cast(Yogurt_Recorder_t *obj) {
  return *reinterpret_cast<std::shared_ptr<Yogurt::RecorderImpl> *>(obj);
}

Yogurt_Recorder_t *yogurt_cast(std::shared_ptr<Yogurt::RecorderImpl> &obj) {
  return reinterpret_cast<Yogurt_Recorder_t *>(&obj);
}

MilkTea_extern(Yogurt_Recorder_Create, (Yogurt_Recorder_t **obj, uint16_t division), {
  MilkTea_nonnull(obj);
  *obj = yogurt_cast(*new std::shared_ptr<Yogurt::RecorderImpl>(Yogurt::RecorderImpl::Make(division)));
})

MilkTea_extern(Yogurt_Recorder_Destroy, (Yogurt_Recorder_t *obj), {
  MilkTea_nonnull(obj);
  delete &yogurt_cast(obj);
})

MilkTea_extern(Yogurt_Recorder_GetState, (Yogurt_Recorder_t *obj, Yogurt_Recorder_State_t *state), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(state);
  *state = Yogurt::ToRawType(yogurt_cast(obj)->state());
})

MilkTea_extern(Yogurt_Recorder_Resume, (Yogurt_Recorder_t *obj), {
  MilkTea_nonnull(obj);
  yogurt_cast(obj)->Resume();
})

MilkTea_extern(Yogurt_Recorder_Pause, (Yogurt_Recorder_t *obj), {
  MilkTea_nonnull(obj);
  yogurt_cast(obj)->Pause();
})

MilkTea_extern(Yogurt_Recorder_Take, (Yogurt_Recorder_t *obj, MilkPowder_Midi_t **midi), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(midi);
  *midi = yogurt_cast(obj)->Take().release();
})

MilkTea_extern(Yogurt_Recorder_Factory_Create, (Yogurt_Recorder_t *obj, uint32_t tempo, SoyBean_Factory_t *factory), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(factory);
  *factory = Yogurt::FactoryImpl(yogurt_cast(obj), tempo).ToRawType();
})
