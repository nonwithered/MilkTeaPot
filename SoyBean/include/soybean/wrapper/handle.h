#ifndef LIB_SOYBEAN_WRAPPER_HANDLE_H_
#define LIB_SOYBEAN_WRAPPER_HANDLE_H_

#include <soybean/common.h>

namespace SoyBean {

class HandleWrapper final : public BaseHandle {
  static constexpr char TAG[] = "SoyBean::HandleWrapper";
  using raw_type = SoyBean_Handle_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  HandleWrapper(raw_type another = {}) : obj_(another) {}
  HandleWrapper(HandleWrapper &&another) : HandleWrapper(another.release()) {}
  ~HandleWrapper() final {
    auto obj = release();
    if (obj.obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyBean_Handle_Destroy, obj);
  }
  BaseHandle &Move() && final {
    return *new HandleWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_NoteOff, get(), channel, note, pressure);
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_NoteOn, get(), channel, note, pressure);
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_AfterTouch, get(), channel, note, pressure);
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    MilkTea_invoke_panic(SoyBean_Handle_ControlChange, get(), channel, control, argument);
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    MilkTea_invoke_panic(SoyBean_Handle_ProgramChange, get(), channel, program);
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    MilkTea_invoke_panic(SoyBean_Handle_ChannelPressure, get(), channel, pressure);
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    MilkTea_invoke_panic(SoyBean_Handle_PitchBend, get(), channel, low, height);
  }
  raw_type release() {
    raw_type result = {};
    std::swap(obj_, result);
    return result;
  }
  raw_type get() {
    return obj_;
  }
 private:
  raw_type obj_;
  MilkTea_NonCopy(HandleWrapper)
  MilkTea_NonMoveAssign(HandleWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_HANDLE_H_
