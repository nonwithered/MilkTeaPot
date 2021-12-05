#include <soybean/common.h>

namespace {

constexpr char TAG[] = "SoyBean";

} // namespace

extern "C" {

MilkTea_extern(SoyBean_Handle_Create, (SoyBean_Handle_t *self, SoyBean_Factory_t factory), {
  MilkTea_nonnull(self);
  factory.interface_->Create(factory.self_, self);
})

MilkTea_extern(SoyBean_Handle_Destroy, (SoyBean_Handle_t self), {
  return self.interface_->Deleter(self.self_);
})

MilkTea_extern(SoyBean_Handle_NoteOff, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->NoteOff(self.self_, channel, note, pressure);
})

MilkTea_extern(SoyBean_Handle_NoteOn, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->NoteOn(self.self_, channel, note, pressure);
})

MilkTea_extern(SoyBean_Handle_AfterTouch, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->AfterTouch(self.self_, channel, note, pressure);
})

MilkTea_extern(SoyBean_Handle_ControlChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t control, uint8_t argument), {
  return self.interface_->ControlChange(self.self_, channel, control, argument);
})

MilkTea_extern(SoyBean_Handle_ProgramChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t program), {
  return self.interface_->ProgramChange(self.self_, channel, program);
})

MilkTea_extern(SoyBean_Handle_ChannelPressure, (SoyBean_Handle_t self, uint8_t channel, uint8_t pressure), {
  return self.interface_->ChannelPressure(self.self_, channel, pressure);
})

MilkTea_extern(SoyBean_Handle_PitchBend, (SoyBean_Handle_t self, uint8_t channel, uint8_t low, uint8_t height), {
  return self.interface_->PitchBend(self.self_, channel, low, height);
})

MilkTea_extern(SoyBean_Factory_Destroy, (SoyBean_Factory_t self), {
  self.interface_->Deleter(self.self_);
})

} // extern "C"
