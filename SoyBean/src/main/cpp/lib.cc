#include <soybean.h>

namespace {

constexpr char TAG[] = "SoyBean_API";

} // namespace

extern "C" {

MilkTea_IMPL(SoyBean_Handle_Create, (SoyBean_Handle_t *self, void *factory, MilkTea_Exception_t (*callback)(void *factory, SoyBean_Handle_t *handle)), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(callback);
  return callback(factory, self);
})

MilkTea_IMPL(SoyBean_Handle_Destroy, (SoyBean_Handle_t self), {
  return self.interface_->deletor(self.handle_);
})

MilkTea_IMPL(SoyBean_Handle_NoteOff, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->note_off(self.handle_, channel, note, pressure);
})

MilkTea_IMPL(SoyBean_Handle_NoteOn, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->note_on(self.handle_, channel, note, pressure);
})

MilkTea_IMPL(SoyBean_Handle_AfterTouch, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->after_touch(self.handle_, channel, note, pressure);
})

MilkTea_IMPL(SoyBean_Handle_ControlChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t control, uint8_t argument), {
  return self.interface_->control_change(self.handle_, channel, control, argument);
})

MilkTea_IMPL(SoyBean_Handle_ProgramChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t program), {
  return self.interface_->program_change(self.handle_, channel, program);
})

MilkTea_IMPL(SoyBean_Handle_ChannelPressure, (SoyBean_Handle_t self, uint8_t channel, uint8_t pressure), {
  return self.interface_->channel_pressure(self.handle_, channel, pressure);
})

MilkTea_IMPL(SoyBean_Handle_PitchBend, (SoyBean_Handle_t self, uint8_t channel, uint8_t low, uint8_t height), {
  return self.interface_->pitch_bend(self.handle_, channel, low, height);
})

} // extern "C"
