#include <soybean.h>

#include "util.h"

MilkTea_Exception_What_IMPL(SoyBean)

extern "C"
SoyBean_API const char *
SoyBean_Exception_What() {
  return SoyBean::MilkTea_Exception_What();
}

#define API_IMPL(section, list, block) \
SoyBean_API SoyBean_Exception_t \
section list { \
  WITH_THROW(block) \
}

namespace {

constexpr char TAG[] = "SoyBean_API";

} // namespace

extern "C" {

API_IMPL(SoyBean_Handle_Create, (SoyBean_Handle_t *self, void *factory, SoyBean_Exception_t (*callback)(void *factory, SoyBean_Handle_t *handle)), {
  THROW_NULL(self);
  THROW_NULL(callback);
  return callback(factory, self);
})

API_IMPL(SoyBean_Handle_Destroy, (SoyBean_Handle_t self), {
  return self.interface_->deletor(self.handle_);
})

API_IMPL(SoyBean_Handle_NoteOff, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->note_off(self.handle_, channel, note, pressure);
})

API_IMPL(SoyBean_Handle_NoteOn, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->note_on(self.handle_, channel, note, pressure);
})

API_IMPL(SoyBean_Handle_AfterTouch, (SoyBean_Handle_t self, uint8_t channel, uint8_t note, uint8_t pressure), {
  return self.interface_->after_touch(self.handle_, channel, note, pressure);
})

API_IMPL(SoyBean_Handle_ControlChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t control, uint8_t argument), {
  return self.interface_->control_change(self.handle_, channel, control, argument);
})

API_IMPL(SoyBean_Handle_ProgramChange, (SoyBean_Handle_t self, uint8_t channel, uint8_t program), {
  return self.interface_->program_change(self.handle_, channel, program);
})

API_IMPL(SoyBean_Handle_ChannelPressure, (SoyBean_Handle_t self, uint8_t channel, uint8_t pressure), {
  return self.interface_->channel_pressure(self.handle_, channel, pressure);
})

API_IMPL(SoyBean_Handle_PitchBend, (SoyBean_Handle_t self, uint8_t channel, uint8_t low, uint8_t height), {
  return self.interface_->pitch_bend(self.handle_, channel, low, height);
})

} // extern "C"
