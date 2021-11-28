#ifndef LIB_SOYBEAN_FACTORY_H_
#define LIB_SOYBEAN_FACTORY_H_

#ifdef __cplusplus
#include <soybean/core.h>
#include <functional>
#include <memory>
namespace SoyBean {

class BaseHandle {
 public:
  SoyBean_Handle_t ToRawType() && {
    return SoyBean_Handle_t{
      .self_ = std::forward<BaseHandle>(*this).Move().release(),
      .interface_ = &Interface(),
    };
  }
  virtual std::unique_ptr<BaseHandle> Move() && = 0;
  virtual ~BaseHandle() = default;
  virtual void Destroy() && = 0;
  virtual void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) = 0;
  virtual void ProgramChange(uint8_t channel, uint8_t program) = 0;
  virtual void ChannelPressure(uint8_t channel, uint8_t pressure) = 0;
  virtual void PitchBend(uint8_t channel, uint8_t low, uint8_t height) = 0;
 private:
  static MilkTea_API const SoyBean_Handle_Interface_t & MilkTea_CALL Interface();
};

class HandleWrapper final : public BaseHandle {
 public:
  static HandleWrapper FromRawType(SoyBean_Handle_t &&another) {
    HandleWrapper self{};
    std::swap(self.self_, another);
    return self;
  }
  HandleWrapper(HandleWrapper &&another) : HandleWrapper() {
    std::swap(self_, another.self_);
  }
  std::unique_ptr<BaseHandle> Move() && final {
    return std::make_unique<HandleWrapper>(std::forward<HandleWrapper>(*this));
  }
  ~HandleWrapper() final {
    if (self_.self_ == nullptr) {
      return;
    }
    MilkTea_panic(SoyBean_Handle_Destroy(self_));
    self_ = {};
  }
  void Destroy() && final {
    delete this;
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
  HandleWrapper() : self_{} {}
  SoyBean_Handle_t self_;
  MilkTea_NonCopy(HandleWrapper)
  MilkTea_NonMoveAssign(HandleWrapper)
  static constexpr char TAG[] = "SoyBean#HandleWrapper";
};

class BaseFactory {
 public:
  SoyBean_Factory_t ToRawType() && {
    return SoyBean_Factory_t{
      .self_ = std::forward<BaseFactory>(*this).Move().release(),
      .interface_ = &Interface(),
    };
  }
  virtual std::unique_ptr<BaseFactory> Move() && = 0;
  virtual ~BaseFactory() = default;
  virtual void Destroy() && = 0;
  virtual std::unique_ptr<BaseHandle> Create() = 0;
 private:
  static MilkTea_API const SoyBean_Factory_Interface_t & MilkTea_CALL Interface();
};

class FactoryWrapper final : public BaseFactory {
 public:
  static FactoryWrapper FromRawType(SoyBean_Factory_t &&another) {
    FactoryWrapper self{};
    std::swap(self.self_, another);
    return self;
  }
  FactoryWrapper(FactoryWrapper &&another) : FactoryWrapper() {
    std::swap(self_, another.self_);
  }
  std::unique_ptr<BaseFactory> Move() && final {
    return std::make_unique<FactoryWrapper>(std::forward<FactoryWrapper>(*this));
  }
  ~FactoryWrapper() final {
    if (self_.self_ == nullptr) {
      return;
    }
    MilkTea_panic(SoyBean_Factory_Destroy(self_));
    self_ = {};
  }
  void Destroy() && final {
    delete this;
  }
  std::unique_ptr<BaseHandle> Create() final {
    SoyBean_Handle_t handle{};
    MilkTea_panic(SoyBean_Handle_Create(&handle, self_));
    return std::make_unique<HandleWrapper>(HandleWrapper::FromRawType(std::forward<SoyBean_Handle_t>(handle)));
  }
private:
  FactoryWrapper() : self_{} {}
  SoyBean_Factory_t self_;
  MilkTea_NonCopy(FactoryWrapper)
  MilkTea_NonMoveAssign(FactoryWrapper)
  static constexpr char TAG[] = "SoyBean#FactoryWrapper";
};

} // namespace SoyBean
#endif // ifdef __cplusplus

#endif // ifndef LIB_SOYBEAN_FACTORY_H_
