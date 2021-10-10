#include <soybean.h>

#include "util.h"

namespace SoyBean {

void BaseHandle::ToRawHandle(SoyBean_Handle_t &handle) {
  static SoyBean_Interface_t interface_ = {
    .deletor = [](void *self) -> MilkTea_Exception_t WITH_THROW({
      delete reinterpret_cast<BaseHandle *>(self);
    }),
    .note_off = [](void *self, uint8_t channel, uint8_t note, uint8_t pressure) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->NoteOff(channel, note, pressure);
    }),
    .note_on = [](void *self, uint8_t channel, uint8_t note, uint8_t pressure) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->NoteOn(channel, note, pressure);
    }),
    .after_touch = [](void *self, uint8_t channel, uint8_t note, uint8_t pressure) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->AfterTouch(channel, note, pressure);
    }),
    .control_change = [](void *self, uint8_t channel, uint8_t control, uint8_t argument) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->ControlChange(channel, control, argument);
    }),
    .program_change = [](void *self, uint8_t channel, uint8_t program) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->ProgramChange(channel, program);
    }),
    .channel_pressure = [](void *self, uint8_t channel, uint8_t pressure) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->ChannelPressure(channel, pressure);
    }),
    .pitch_bend = [](void *self, uint8_t channel, uint8_t low, uint8_t height) -> MilkTea_Exception_t WITH_THROW({
      reinterpret_cast<BaseHandle *>(self)->PitchBend(channel, low, height);
    }),
  };
  handle.handle_ = this;
  handle.interface_ = &interface_;
}

MilkTea_Exception_t (*BaseFactory::Callback())(void *factory, SoyBean_Handle_t *handle) {
  static MilkTea_Exception_t (*callback_)(void *factory, SoyBean_Handle_t *handle) = [](void *factory, SoyBean_Handle_t *handle) -> MilkTea_Exception_t WITH_THROW({
    THROW_NULL(factory);
    reinterpret_cast<BaseFactory *>(factory)->Create()->ToRawHandle(*handle);
  });
  return callback_;
}

} // namespace SoyBean
