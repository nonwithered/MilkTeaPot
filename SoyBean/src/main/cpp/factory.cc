#include <soybean.h>

namespace SoyBean {

SoyBean_Handle_t BaseHandle::ToRawHandle() {
  static SoyBean_Interface_t interface_ = {
    .deletor = [](void *self) -> MilkTea_Exception_t MilkTea_with_except({
      delete reinterpret_cast<BaseHandle *>(self);
    }),
    .note_off = [](void *self, uint8_t channel, uint8_t note, uint8_t pressure) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->NoteOff(channel, note, pressure);
    }),
    .note_on = [](void *self, uint8_t channel, uint8_t note, uint8_t pressure) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->NoteOn(channel, note, pressure);
    }),
    .after_touch = [](void *self, uint8_t channel, uint8_t note, uint8_t pressure) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->AfterTouch(channel, note, pressure);
    }),
    .control_change = [](void *self, uint8_t channel, uint8_t control, uint8_t argument) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->ControlChange(channel, control, argument);
    }),
    .program_change = [](void *self, uint8_t channel, uint8_t program) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->ProgramChange(channel, program);
    }),
    .channel_pressure = [](void *self, uint8_t channel, uint8_t pressure) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->ChannelPressure(channel, pressure);
    }),
    .pitch_bend = [](void *self, uint8_t channel, uint8_t low, uint8_t height) -> MilkTea_Exception_t MilkTea_with_except({
      reinterpret_cast<BaseHandle *>(self)->PitchBend(channel, low, height);
    }),
  };
  return SoyBean_Handle_t{
    .handle_ = this,
    .interface_ = &interface_,
  };
}

} // namespace SoyBean
