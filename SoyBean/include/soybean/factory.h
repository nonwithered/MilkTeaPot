#ifndef LIB_SOYBEAN_FACTORY_H_
#define LIB_SOYBEAN_FACTORY_H_

#ifdef __cplusplus
#include <soybean/core.h>
#include <functional>
namespace SoyBean {

class BaseHandle {
 public:
  MilkTea_API SoyBean_Handle_t MilkTea_CALL ToRawHandle();
  virtual ~BaseHandle() = default;
  virtual void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) = 0;
  virtual void ProgramChange(uint8_t channel, uint8_t program) = 0;
  virtual void ChannelPressure(uint8_t channel, uint8_t pressure) = 0;
  virtual void PitchBend(uint8_t channel, uint8_t low, uint8_t height) = 0;
};

class HandleWrapper final : public BaseHandle {
 public:
  static BaseHandle *FromRawHandle(const SoyBean_Handle_t &handle) {
    return new HandleWrapper(handle);
  }
  ~HandleWrapper() final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_Destroy(handle_));
    handle_.handle_ = nullptr;
    handle_.interface_ = nullptr;
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_NoteOff(handle_, channel, note, pressure));
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_NoteOn(handle_, channel, note, pressure));
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_AfterTouch(handle_, channel, note, pressure));
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_ControlChange(handle_, channel, control, argument));
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_ProgramChange(handle_, channel, program));
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_ChannelPressure(handle_, channel, pressure));
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    MilkTea::Exception::ThrowFromRawType(SoyBean_Handle_PitchBend(handle_, channel, low, height));
  }
 private:
  HandleWrapper(const SoyBean_Handle_t &handle) : handle_(handle) {}
  SoyBean_Handle_t handle_;
};

class BaseFactory {
 public:
  MilkTea_API SoyBean_Factory_t MilkTea_CALL ToRawFactory();
  virtual ~BaseFactory() = default;
  virtual BaseHandle *Create() = 0;
};

} // namespace SoyBean
#endif // ifdef __cplusplus

#endif // ifndef LIB_SOYBEAN_FACTORY_H_
