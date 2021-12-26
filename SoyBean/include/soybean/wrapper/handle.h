#ifndef LIB_SOYBEAN_WRAPPER_HANDLE_H_
#define LIB_SOYBEAN_WRAPPER_HANDLE_H_

#include <soybean/common.h>

namespace SoyBean {

class BaseHandle {
 public:
  virtual SoyBean_Handle_t ToRawType() && {
    return SoyBean_Handle_t{
      .self_ = &std::forward<BaseHandle>(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseHandle() = default;
  virtual BaseHandle &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) = 0;
  virtual void ProgramChange(uint8_t channel, uint8_t program) = 0;
  virtual void ChannelPressure(uint8_t channel, uint8_t pressure) = 0;
  virtual void PitchBend(uint8_t channel, uint8_t low, uint8_t height) = 0;
 private:
  static MilkTea_decl(const SoyBean_Handle_Interface_t &) Interface();
};

class HandleWrapper final : public BaseHandle {
  static constexpr char TAG[] = "SoyBean::HandleWrapper";
 public:
  SoyBean_Handle_t ToRawType() && final {
    return release();
  }
  HandleWrapper(SoyBean_Handle_t another = {}) : self_(another) {}
  HandleWrapper(HandleWrapper &&another) : HandleWrapper() {
    std::swap(self_, another.self_);
  }
  ~HandleWrapper() final {
    if (self_.self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyBean_Handle_Destroy, self_);
    self_ = {};
  }
  BaseHandle &Move() && final {
    return *new HandleWrapper(std::forward<HandleWrapper>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_NoteOff, self_, channel, note, pressure);
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_NoteOn, self_, channel, note, pressure);
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_AfterTouch, self_, channel, note, pressure);
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    MilkTea_invoke_panic(SoyBean_Handle_ControlChange, self_, channel, control, argument);
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    MilkTea_invoke_panic(SoyBean_Handle_ProgramChange, self_, channel, program);
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_ChannelPressure, self_, channel, pressure);
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    MilkTea_invoke_panic(SoyBean_Handle_PitchBend, self_, channel, low, height);
  }
  SoyBean_Handle_t release() {
    SoyBean_Handle_t self = self_;
    self_ = {};
    return self;
  }
 private:
  SoyBean_Handle_t self_;
  MilkTea_NonCopy(HandleWrapper)
  MilkTea_NonMoveAssign(HandleWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_HANDLE_H_
