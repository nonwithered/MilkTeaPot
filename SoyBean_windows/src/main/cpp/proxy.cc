#include "proxy.h"

#include <windows.h>

#include <string>
#include <sstream>
#include <array>
#include <cstdint>

namespace SoyBean_Windows {

struct Proxy_HMIDIOUT_t {
  HMIDIOUT handler_;
};

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT *p, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen) {
  *p = new Proxy_HMIDIOUT_t();
  return midiOutOpen((LPHMIDIOUT) &(*p)->handler_, (UINT) uDeviceID, (DWORD_PTR) dwCallback, (DWORD_PTR) dwInstance, (DWORD) fdwOpen);
}

Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT p) {
  Proxy_MMRESULT r = midiOutClose(p->handler_);
  delete p;
  return r;
}

Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT p, uint32_t msg) {
  return midiOutShortMsg(p->handler_, msg);
}

std::string Proxy_midiInGetErrorTextA(Proxy_MMRESULT result) {
  std::stringstream ss;
  ss << "MMRESULT";
  ss << " ";
  ss << result;
  const char *s = Proxy_MMRESULT_What(result);
  if (s != nullptr) {
    ss << " ";
    ss << s;
  }
  std::array<char, 128> arr;
  auto r = midiInGetErrorTextA(result, (LPSTR) arr.data(), (UINT) 128);
  if (r == MMSYSERR_NOERROR) {
    ss << " ";
    ss << arr.data();
  }
  return ss.str();
}

} // namespace SoyBean_Windows
