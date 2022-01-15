#include <soybean/common.h>

namespace {

constexpr char TAG[] = "SoyBean";

SoyBean::BaseHandle &BaseHandle_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyBean::BaseHandle *>(self);
}

SoyBean::BaseFactory &BaseFactory_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyBean::BaseFactory *>(self);
}

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseHandle_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_NoteOff(void *self, uint8_t channel, uint8_t note, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).NoteOff(channel, note, pressure);
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_NoteOn(void *self, uint8_t channel, uint8_t note, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).NoteOn(channel, note, pressure);
});

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_AfterTouch(void *self, uint8_t channel, uint8_t note, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).AfterTouch(channel, note, pressure);
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_ControlChange(void *self, uint8_t channel, uint8_t control, uint8_t argument) MilkTea_with_except({
  BaseHandle_cast(self).ControlChange(channel, control, argument);
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_ProgramChange(void *self, uint8_t channel, uint8_t program) MilkTea_with_except({
  BaseHandle_cast(self).ProgramChange(channel, program);
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_ChannelPressure(void *self, uint8_t channel, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).ChannelPressure(channel, pressure);
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_PitchBend(void *self, uint8_t channel, uint8_t low, uint8_t height) MilkTea_with_except({
  BaseHandle_cast(self).PitchBend(channel, low, height);
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseFactory_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseFactory_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseFactory_Interface_Create(void *self, SoyBean_Handle_t *handle) MilkTea_with_except({
  MilkTea_nonnull(handle);
  auto *handle_ = BaseFactory_cast(self).Create().release();
  MilkTea::Defer defer([handle_]() {
    std::move(*handle_).Destroy();
  });
  *handle = std::move(*handle_).ToRawType();
})

} // namespace

namespace SoyBean {

const BaseHandle::interface_type &BaseHandle::Interface() {
  static const interface_type instance_{
    .Deleter = SoyBean_BaseHandle_Interface_Deleter,
    .NoteOff = SoyBean_BaseHandle_Interface_NoteOff,
    .NoteOn = SoyBean_BaseHandle_Interface_NoteOn,
    .AfterTouch = SoyBean_BaseHandle_Interface_AfterTouch,
    .ControlChange = SoyBean_BaseHandle_Interface_ControlChange,
    .ProgramChange = SoyBean_BaseHandle_Interface_ProgramChange,
    .ChannelPressure = SoyBean_BaseHandle_Interface_ChannelPressure,
    .PitchBend = SoyBean_BaseHandle_Interface_PitchBend,
  };
  return instance_;
}

const BaseFactory::interface_type &BaseFactory::Interface() {
  static const interface_type instance_{
    .Deleter = SoyBean_BaseFactory_Interface_Deleter,
    .Create = SoyBean_BaseFactory_Interface_Create,
  };
  return instance_;
}

} // namespace SoyBean
