#include "proxy.h"

#include <windows.h>

#include <string>
#include <sstream>
#include <array>
#include <cstdint>

namespace SoyBean_Windows {

struct Proxy_HMIDIOUT_t {
  Proxy_HMIDIOUT_t(HMIDIOUT handler) : handler_(handler) {}
  HMIDIOUT handler_;
};

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT *p, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen) {
  Proxy_HMIDIOUT_t q = nullptr;
  Proxy_MMRESULT r = midiOutOpen((LPHMIDIOUT) &q.handler_, (UINT) uDeviceID, (DWORD_PTR) dwCallback, (DWORD_PTR) dwInstance, (DWORD) fdwOpen);
  if (r == Proxy_MMSYSERR_NOERROR) {
    *p = new Proxy_HMIDIOUT_t(q.handler_);
  }
  return r;
}

Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT p) {
  Proxy_MMRESULT r = midiOutClose(p->handler_);
  if (r == Proxy_MMSYSERR_NOERROR) {
    delete p;
  }
  return r;
}

Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT p, uint32_t msg) {
  return midiOutShortMsg(p->handler_, msg);
}

std::string Proxy_midiInGetErrorText(Proxy_MMRESULT result) {
  std::stringstream ss;
  ss << "MMRESULT";
  ss << " ";
  ss << result;
  std::array<char, 128> arr;
  auto r = midiInGetErrorText(result, (LPSTR) arr.data(), (UINT) 128);
  if (r == Proxy_MMSYSERR_NOERROR) {
    ss << " ";
    ss << arr.data();
  }
  return ss.str();
}

} // namespace SoyBean_Windows
