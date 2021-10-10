#include "wrapper.h"

#include <windows.h>

namespace SoyBean_Windows {

struct Proxy_HMIDIOUT_t {
  HMIDIOUT handler_;
};

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT *p) {
  *p = new Proxy_HMIDIOUT_t();
  return midiOutOpen(&(*p)->handler_, 0, 0, 0, 0);
}

Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT p) {
  Proxy_MMRESULT r = midiOutClose(p->handler_);
  delete p;
  return r;
}

Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT p, uint32_t msg) {
  return midiOutShortMsg(p->handler_, msg);
}

} // namespace SoyBean_Windows
