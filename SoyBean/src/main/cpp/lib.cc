#include <soybean.h>

namespace {

constexpr char TAG[] = "SoyBean#extern";

} // namespace

extern "C" {

MilkTea_IMPL(SoyBean_Handle_Create, (SoyBean_Handle_t *self, SoyBean_Factory_t factory), {
  MilkTea_nonnull(self);
  factory.interface_->Create(factory.factory_, self);
})

MilkTea_IMPL(SoyBean_Handle_Destroy, (SoyBean_Handle_t self), {
  return self.interface_->Deletor(self.handle_);
})

MilkTea_IMPL(SoyBean_Handle_NoteOff, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->NoteOff(self.handle_, channel, note, pressure);
})

MilkTea_IMPL(SoyBean_Handle_NoteOn, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->NoteOn(self.handle_, channel, note, pressure);
})

MilkTea_IMPL(SoyBean_Handle_AfterTouch, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->AfterTouch(self.handle_, channel, note, pressure);
})

MilkTea_IMPL(SoyBean_Handle_ControlChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t control, uint8_t argument), {
  return self.interface_->ControlChange(self.handle_, channel, control, argument);
})

MilkTea_IMPL(SoyBean_Handle_ProgramChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t program), {
  return self.interface_->ProgramChange(self.handle_, channel, program);
})

MilkTea_IMPL(SoyBean_Handle_ChannelPressure, (SoyBean_Handle_t self, uint8_t channel, uint8_t pressure), {
  return self.interface_->ChannelPressure(self.handle_, channel, pressure);
})

MilkTea_IMPL(SoyBean_Handle_PitchBend, (SoyBean_Handle_t self, uint8_t channel, uint8_t low, uint8_t height), {
  return self.interface_->PitchBend(self.handle_, channel, low, height);
})

MilkTea_IMPL(SoyBean_Factory_Destroy, (SoyBean_Factory_t self), {
  self.interface_->Deletor(self.factory_);
})

} // extern "C"
