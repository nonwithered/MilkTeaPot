#ifndef SOYBEAN_WINDOWS_HANDLE_H_
#define SOYBEAN_WINDOWS_HANDLE_H_

#include <soybean.h>

#include "util.h"
#include "wrapper.h"

namespace SoyBean_Windows {

class HandleImpl final : public SoyBean::BaseHandle {
 public:
  HandleImpl() : handle_(nullptr), closed_(false) {
    ThrowOrNot(Proxy_midiOutOpen(&handle_));
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
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0x80 | channel, note, pressure)));
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0x90 | channel, note, pressure)));
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xa0 | channel, note, pressure)));
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    CheckClosed();
    CheckChannel(channel);
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xb0 | channel, control, argument)));
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    CheckClosed();
    CheckChannel(channel);
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xc0 | channel, program, 0)));
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    CheckClosed();
    CheckChannel(channel);
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xd0 | channel, pressure, 0)));
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    CheckClosed();
    CheckChannel(channel);
    ThrowOrNot(Proxy_midiOutShortMsg(handle_, Dword(0xe0 | channel, low, height)));
  }
 private:
  void CheckClosed() const {
    if (closed_) {
      LOG_PRINT(ERROR, TAG, "handle is closed");
      THROW(LogicError, "handle is closed");
    }
  }
  static void CheckChannel(uint8_t channel) {
    if (channel & 0xf0 != 0) {
      LOG_PRINT(ERROR, TAG, "channel should not large than 0x0f but this value is %02" PRIx8, channel);
      THROW(LogicError, "handle is closed");
    }
  }
  static uint32_t Dword(uint8_t type, uint8_t arg_0, uint8_t arg_1) {
    uint8_t msg[4] = {type, arg_0, arg_1, 0};
    return *reinterpret_cast<uint32_t *>(&msg);
  }
  static void ThrowOrNot(Wrapper::Proxy_MMRESULT r) {
    if (r == 0) {
      return;
    }
    const char *what = Wrapper::Proxy_MMRESULT_What(r);
    if (what != nullptr) {
      THROW(Unknown, what);
    } else {
      THROW_FORMAT(Unknown, "MMRESULT %u", r);
    }
  }
  Wrapper::Proxy_HMIDIOUT handle_;
  bool closed_;
  static constexpr char TAG[] = "handle";
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_HANDLE_H_
