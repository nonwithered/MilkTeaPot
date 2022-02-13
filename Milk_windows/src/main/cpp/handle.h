#ifndef MILK_WINDOWS_HANDLE_H_
#define MILK_WINDOWS_HANDLE_H_

#include <array>
#include <functional>

#include <soybean.h>

namespace Milk_Windows {

class HandleImpl final : public SoyBean::BaseHandle {
  static constexpr size_t msg_size = 4;
 public:
  explicit HandleImpl(std::function<void(uint8_t, uint8_t, uint8_t, uint8_t)> consumer) : consumer_(std::move(consumer)) {}
  HandleImpl(HandleImpl &&another) : consumer_(std::move(another.consumer_)) {}
  ~HandleImpl() = default;
  HandleImpl &Move() && final {
    return *new HandleImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    SendMsg(0x80, channel, note, pressure);
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    SendMsg(0x90, channel, note, pressure);
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    SendMsg(0xa0, channel, note, pressure);
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    SendMsg(0xb0, channel, control, argument);
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    SendMsg(0xc0, channel, program);
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    SendMsg(0xd0, channel, pressure);
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    SendMsg(0xe0, channel, low, height);
  }
 private:
  void SendMsg(uint8_t type, uint8_t channel, uint8_t arg0, uint8_t arg1 = 0) {
    if (!consumer_) {
      return;
    }
    consumer_(type, channel, arg0, arg1);
  }
  std::function<void(uint8_t, uint8_t, uint8_t, uint8_t)> consumer_;
  MilkTea_NonCopy(HandleImpl)
  MilkTea_NonMoveAssign(HandleImpl)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_HANDLE_H_
