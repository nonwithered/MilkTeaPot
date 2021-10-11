#ifndef SOYBEAN_WINDOWS_WRAPPER_H_
#define SOYBEAN_WINDOWS_WRAPPER_H_

#include <cstdint>

namespace SoyBean_Windows {

struct Proxy_HMIDIOUT_t;
using Proxy_HMIDIOUT = Proxy_HMIDIOUT_t *;

using Proxy_MMRESULT = unsigned int;

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT *p, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen);
Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT p);
Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT p, uint32_t msg);

constexpr Proxy_MMRESULT Proxy_MIDIERR_BASE = 64;
constexpr Proxy_MMRESULT Proxy_MIDIERR_UNPREPARED = Proxy_MIDIERR_BASE + 0;
constexpr Proxy_MMRESULT Proxy_MIDIERR_STILLPLAYING = Proxy_MIDIERR_BASE + 1;
constexpr Proxy_MMRESULT Proxy_MIDIERR_NOMAP = Proxy_MIDIERR_BASE + 2;
constexpr Proxy_MMRESULT Proxy_MIDIERR_NOTREADY = Proxy_MIDIERR_BASE + 3;
constexpr Proxy_MMRESULT Proxy_MIDIERR_NODEVICE = Proxy_MIDIERR_BASE + 4;
constexpr Proxy_MMRESULT Proxy_MIDIERR_INVALIDSETUP = Proxy_MIDIERR_BASE + 5;
constexpr Proxy_MMRESULT Proxy_MIDIERR_BADOPENMODE = Proxy_MIDIERR_BASE + 6;
constexpr Proxy_MMRESULT Proxy_MIDIERR_DONT_CONTINUE = Proxy_MIDIERR_BASE + 7;

inline const char *Proxy_MMRESULT_What(Proxy_MMRESULT result) {
  switch (result) {
    case Proxy_MIDIERR_UNPREPARED: return "MIDIERR_UNPREPARED";
    case Proxy_MIDIERR_STILLPLAYING: return "MIDIERR_STILLPLAYING";
    case Proxy_MIDIERR_NOMAP: return "MIDIERR_NOMAP";
    case Proxy_MIDIERR_NOTREADY: return "MIDIERR_NOTREADY";
    case Proxy_MIDIERR_NODEVICE: return "MIDIERR_NODEVICE";
    case Proxy_MIDIERR_INVALIDSETUP: return "MIDIERR_INVALIDSETUP";
    case Proxy_MIDIERR_BADOPENMODE: return "MIDIERR_BADOPENMODE";
    case Proxy_MIDIERR_DONT_CONTINUE: return "MIDIERR_DONT_CONTINUE";
    default: return nullptr;
  }
}

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_WRAPPER_H_
