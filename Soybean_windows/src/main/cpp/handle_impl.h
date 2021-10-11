#ifndef SOYBEAN_WINDOWS_HANDLE_H_
#define SOYBEAN_WINDOWS_HANDLE_H_

#include <soybean.h>

#include "wrapper.h"

namespace SoyBean_Windows {

class HandleImpl final : public SoyBean::BaseHandle {
 public:
  HandleImpl(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen) : handle_(nullptr), closed_(false) {
    ThrowOrNot(Proxy_midiOutOpen(&handle_, uDeviceID, dwCallback, dwInstance, fdwOpen));
  }
  ~HandleImpl() final {
    CheckClosed();
    closed_ = true;
    ThrowOrNot(Proxy_midiOutClose(handle_));
    handle_ = nullptr;
  }
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(note, "note");
    CheckArgs(pressure, "pressure");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0x80 | channel, note, pressure)));
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(note, "note");
    CheckArgs(pressure, "pressure");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0x90 | channel, note, pressure)));
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(note, "note");
    CheckArgs(pressure, "pressure");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xa0 | channel, note, pressure)));
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(control, "control");
    CheckArgs(argument, "argument");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xb0 | channel, control, argument)));
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(program, "program");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xc0 | channel, program, 0)));
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(pressure, "pressure");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xd0 | channel, pressure, 0)));
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    CheckClosed();
    CheckChannel(channel);
    CheckArgs(low, "low");
    CheckArgs(height, "height");
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xe0 | channel, low, height)));
  }
 private:
  void CheckClosed() const {
    if (closed_) {
      MilkTea_LogPrint(ERROR, TAG, "handle is closed");
      MilkTea_throw(LogicError, "handle is closed");
    }
  }
  static void CheckChannel(uint8_t channel) {
    if (channel > 0x0f) {
      MilkTea_LogPrint(ERROR, TAG, "channel should not large than 0x0f but this value is 0x%02" PRIx8 " now", channel);
      MilkTea_throwf(InvalidParam, "channel should not large than 0x0f but this value is 0x%02" PRIx8 " now", channel);
    }
  }
  static void CheckArgs(uint8_t arg, const char *sym) {
    if (arg > 0x7f) {
      MilkTea_LogPrint(ERROR, TAG, "%s should not large than 0x7f but this value is 0x%02" PRIx8 " now", sym, arg);
      MilkTea_throwf(InvalidParam, "%s should not large than 0x7f but this value is 0x%02" PRIx8 " now", sym, arg);
    }
  }
  static uint32_t Dword(uint8_t type, uint8_t arg_0, uint8_t arg_1) {
    uint8_t msg[4] = {type, arg_0, arg_1, 0};
    return *reinterpret_cast<uint32_t *>(&msg);
  }
  static void ThrowOrNot(Proxy_MMRESULT r) {
    if (r == 0) {
      return;
    }
    const char *what = Proxy_MMRESULT_What(r);
    if (what != nullptr) {
      MilkTea_throw(Unknown, what);
    } else {
      MilkTea_throwf(Unknown, "MMRESULT %u", r);
    }
  }
  Proxy_HMIDIOUT handle_;
  bool closed_;
  static constexpr char TAG[] = "handle";
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_HANDLE_H_
