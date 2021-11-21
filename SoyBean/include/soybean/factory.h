#ifndef LIB_SOYBEAN_FACTORY_H_
#define LIB_SOYBEAN_FACTORY_H_

#ifdef __cplusplus
#include <soybean/core.h>
#include <functional>
#include <memory>
namespace SoyBean {

class BaseHandle {
 public:
  MilkTea_API SoyBean_Handle_t MilkTea_CALL ToRawType();
  virtual ~BaseHandle() = default;
  virtual void Destroy() = 0;
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
  explicit HandleWrapper(SoyBean_Handle_t &&handle) : self_(handle) {}
  ~HandleWrapper() final {
    MilkTea_panic(SoyBean_Handle_Destroy(self_));
    self_.self_ = nullptr;
    self_.interface_ = nullptr;
  }
  void Destroy() final {
    MilkTea_panic(SoyBean_Handle_Destroy(self_));
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_panic(SoyBean_Handle_NoteOff(self_, channel, note, pressure));
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_panic(SoyBean_Handle_NoteOn(self_, channel, note, pressure));
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_panic(SoyBean_Handle_AfterTouch(self_, channel, note, pressure));
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    MilkTea_panic(SoyBean_Handle_ControlChange(self_, channel, control, argument));
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    MilkTea_panic(SoyBean_Handle_ProgramChange(self_, channel, program));
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    MilkTea_panic(SoyBean_Handle_ChannelPressure(self_, channel, pressure));
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    MilkTea_panic(SoyBean_Handle_PitchBend(self_, channel, low, height));
  }
 private:
  SoyBean_Handle_t self_;
  MilkTea_NonCopy(HandleWrapper)
  MilkTea_NonMove(HandleWrapper)
};

class BaseFactory {
 public:
  MilkTea_API SoyBean_Factory_t MilkTea_CALL ToRawType();
  virtual ~BaseFactory() = default;
  virtual void Destroy() = 0;
  virtual std::unique_ptr<BaseHandle> Create() = 0;
};

class FactoryWrapper final : public BaseFactory {
 public:
  explicit FactoryWrapper(SoyBean_Factory_t &&factory) : self_(factory) {}
  ~FactoryWrapper() final {
    MilkTea_panic(SoyBean_Factory_Destroy(self_));
    self_.self_ = nullptr;
    self_.interface_ = nullptr;
  }
  void Destroy() final {
    MilkTea_panic(SoyBean_Factory_Destroy(self_));
  }
  std::unique_ptr<BaseHandle> Create() final {
    SoyBean_Handle_t handle{};
    MilkTea_panic(SoyBean_Handle_Create(&handle, self_));
    return std::make_unique<HandleWrapper>(std::move(handle));
  }
private:
  SoyBean_Factory_t self_;
  MilkTea_NonCopy(FactoryWrapper)
  MilkTea_NonMove(FactoryWrapper)
};

} // namespace SoyBean
#endif // ifdef __cplusplus

#endif // ifndef LIB_SOYBEAN_FACTORY_H_
