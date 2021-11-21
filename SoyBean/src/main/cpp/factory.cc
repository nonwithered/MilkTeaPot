#include <soybean.h>

namespace {

constexpr char TAG[] = "SoyBean#extern";

SoyBean::BaseHandle &BaseHandle_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyBean::BaseHandle *>(self);
}

SoyBean::BaseFactory &BaseFactory_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyBean::BaseFactory *>(self);
}

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_Deleter(void *self) MilkTea_with_except({
  BaseHandle_cast(self).Destroy();
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_NoteOff(void *self, uint8_t channel, uint8_t note, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).NoteOff(channel, note, pressure);
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_NoteOn(void *self, uint8_t channel, uint8_t note, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).NoteOn(channel, note, pressure);
});

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_AfterTouch(void *self, uint8_t channel, uint8_t note, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).AfterTouch(channel, note, pressure);
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_ControlChange(void *self, uint8_t channel, uint8_t control, uint8_t argument) MilkTea_with_except({
  BaseHandle_cast(self).ControlChange(channel, control, argument);
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_ProgramChange(void *self, uint8_t channel, uint8_t program) MilkTea_with_except({
  BaseHandle_cast(self).ProgramChange(channel, program);
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_ChannelPressure(void *self, uint8_t channel, uint8_t pressure) MilkTea_with_except({
  BaseHandle_cast(self).ChannelPressure(channel, pressure);
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Handle_Interface_PitchBend(void *self, uint8_t channel, uint8_t low, uint8_t height) MilkTea_with_except({
  BaseHandle_cast(self).PitchBend(channel, low, height);
})

const SoyBean_Handle_Interface_t &SoyBean_Handle_Interface_Instance() {
  static const SoyBean_Handle_Interface_t interface_{
    .Deleter = SoyBean_Handle_Interface_Deleter,
    .NoteOff = SoyBean_Handle_Interface_NoteOff,
    .NoteOn = SoyBean_Handle_Interface_NoteOn,
    .AfterTouch = SoyBean_Handle_Interface_AfterTouch,
    .ControlChange = SoyBean_Handle_Interface_ControlChange,
    .ProgramChange = SoyBean_Handle_Interface_ProgramChange,
    .ChannelPressure = SoyBean_Handle_Interface_ChannelPressure,
    .PitchBend = SoyBean_Handle_Interface_PitchBend,
  };
  return interface_;
}

MilkTea_Exception_t MilkTea_CALL SoyBean_Factory_Interface_Deleter(void *self) MilkTea_with_except({
  BaseFactory_cast(self).Destroy();
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Factory_Interface_Create(void *self, SoyBean_Handle_t *handle) MilkTea_with_except({
  *handle = BaseFactory_cast(self).Create().release()->ToRawType();
})

const SoyBean_Factory_Interface_t &SoyBean_Factory_Interface_Instance() {
  static const SoyBean_Factory_Interface_t interface_{
    .Deleter = SoyBean_Factory_Interface_Deleter,
    .Create = SoyBean_Factory_Interface_Create,
  };
  return interface_;
}

} // namespace

namespace SoyBean {

SoyBean_Handle_t BaseHandle::ToRawType() {
  return SoyBean_Handle_t{
    .self_ = this,
    .interface_ = &SoyBean_Handle_Interface_Instance(),
  };
}

SoyBean_Factory_t BaseFactory::ToRawType() {
  return SoyBean_Factory_t{
    .self_ = this,
    .interface_ = &SoyBean_Factory_Interface_Instance(),
  };
}

} // namespace SoyBean
