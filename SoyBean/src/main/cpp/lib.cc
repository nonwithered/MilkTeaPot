#include <soybean/common.h>

namespace {

constexpr char TAG[] = "SoyBean";

} // namespace

MilkTea_extern(SoyBean_Handle_Create, (SoyBean_Handle_t *obj, SoyBean_Factory_t factory), {
  MilkTea_nonnull(obj);
  factory.interface_->Create(factory.obj_, obj);
})

MilkTea_extern(SoyBean_Handle_Destroy, (SoyBean_Handle_t obj), {
  return obj.interface_->Deleter(obj.obj_);
})

MilkTea_extern(SoyBean_Handle_NoteOff, (SoyBean_Handle_t obj, uint8_t channel, uint8_t note, uint8_t pressure), {
  return obj.interface_->NoteOff(obj.obj_, channel, note, pressure);
})

MilkTea_extern(SoyBean_Handle_NoteOn, (SoyBean_Handle_t obj, uint8_t channel, uint8_t note, uint8_t pressure), {
  return obj.interface_->NoteOn(obj.obj_, channel, note, pressure);
})

MilkTea_extern(SoyBean_Handle_AfterTouch, (SoyBean_Handle_t obj, uint8_t channel, uint8_t note, uint8_t pressure), {
  return obj.interface_->AfterTouch(obj.obj_, channel, note, pressure);
})

MilkTea_extern(SoyBean_Handle_ControlChange, (SoyBean_Handle_t obj, uint8_t channel, uint8_t control, uint8_t argument), {
  return obj.interface_->ControlChange(obj.obj_, channel, control, argument);
})

MilkTea_extern(SoyBean_Handle_ProgramChange, (SoyBean_Handle_t obj, uint8_t channel, uint8_t program), {
  return obj.interface_->ProgramChange(obj.obj_, channel, program);
})

MilkTea_extern(SoyBean_Handle_ChannelPressure, (SoyBean_Handle_t obj, uint8_t channel, uint8_t pressure), {
  return obj.interface_->ChannelPressure(obj.obj_, channel, pressure);
})

MilkTea_extern(SoyBean_Handle_PitchBend, (SoyBean_Handle_t obj, uint8_t channel, uint8_t low, uint8_t height), {
  return obj.interface_->PitchBend(obj.obj_, channel, low, height);
})

MilkTea_extern(SoyBean_Factory_Destroy, (SoyBean_Factory_t obj), {
  obj.interface_->Deleter(obj.obj_);
})
