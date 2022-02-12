#ifndef SOYBEAN_WINDOWS_HANDLE_H_
#define SOYBEAN_WINDOWS_HANDLE_H_

#include <string_view>

#include <soybean.h>

#include "proxy.h"

namespace SoyBean_Windows {

#define ThrowOrNot_invoke(S, ...)\
ThrowOrNot(S(__VA_ARGS__), #S)

class HandleImpl final : public SoyBean::BaseHandle {
  static constexpr char TAG[] = "SoyBean_Windows::HandleImpl";
 public:
  HandleImpl(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen) : HandleImpl() {
    ThrowOrNot_invoke(Proxy_midiOutOpen, &obj_, uDeviceID, dwCallback, dwInstance, fdwOpen);
  }
  HandleImpl(HandleImpl &&another) : HandleImpl() {
    std::swap(obj_, another.obj_);
  }
  ~HandleImpl() final {
    if (obj_ == nullptr) {
      return;
    }
    ThrowOrNot_invoke(Proxy_midiOutClose, obj_);
    obj_ = nullptr;
  }
  BaseHandle &Move() && final {
    return *new HandleImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckChannel(channel);
    CheckArgs(note, "note");
    CheckArgs(pressure, "pressure");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0x80 | channel, note, pressure));
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckChannel(channel);
    CheckArgs(note, "note");
    CheckArgs(pressure, "pressure");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0x90 | channel, note, pressure));
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckChannel(channel);
    CheckArgs(note, "note");
    CheckArgs(pressure, "pressure");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0xa0 | channel, note, pressure));
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    CheckChannel(channel);
    CheckArgs(control, "control");
    CheckArgs(argument, "argument");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0xb0 | channel, control, argument));
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    CheckChannel(channel);
    CheckArgs(program, "program");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0xc0 | channel, program, 0));
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    CheckChannel(channel);
    CheckArgs(pressure, "pressure");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0xd0 | channel, pressure, 0));
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    CheckChannel(channel);
    CheckArgs(low, "low");
    CheckArgs(height, "height");
    ThrowOrNot_invoke(Proxy_midiOutShortMsg, obj_, Dword(0xe0 | channel, low, height));
  }
 private:
  static void CheckChannel(uint8_t channel) {
    if (channel > 0x0f) {
      MilkTea_throwf(InvalidParam, "CheckChannel -- channel should not large than 0x0f but this value is 0x%02" PRIx8 " now", channel);
    }
  }
  static void CheckArgs(uint8_t arg, const char *sym) {
    if (arg > 0x7f) {
      MilkTea_throwf(InvalidParam, "CheckArgs -- %s should not large than 0x7f but this value is 0x%02" PRIx8 " now", sym, arg);
    }
  }
  static uint32_t Dword(uint8_t type, uint8_t arg_0, uint8_t arg_1) {
    uint8_t msg[4] = {type, arg_0, arg_1, 0};
    return *reinterpret_cast<uint32_t *>(&msg);
  }
  static void ThrowOrNot(Proxy_MMRESULT r, std::string_view tag) {
    if (r == Proxy_MMSYSERR_NOERROR) {
      return;
    }
    auto what = Proxy_midiInGetErrorTextA(r);
    MilkTea_throwf(Unknown, "%s: %s", tag.data(), what.data());
  }
  HandleImpl() : obj_(nullptr) {}
  Proxy_HMIDIOUT obj_;
  MilkTea_NonCopy(HandleImpl)
  MilkTea_NonMoveAssign(HandleImpl)
};

#undef ThrowOrNot_invoke

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_HANDLE_H_
