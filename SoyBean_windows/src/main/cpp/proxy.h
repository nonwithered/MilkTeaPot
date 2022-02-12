#ifndef SOYBEAN_WINDOWS_PROXY_H_
#define SOYBEAN_WINDOWS_PROXY_H_

#include <string>
#include <cstdint>

namespace SoyBean_Windows {

struct Proxy_HMIDIOUT_t;
using Proxy_HMIDIOUT = Proxy_HMIDIOUT_t *;
using Proxy_MMRESULT = unsigned int;

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT *p, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen);
Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT p);
Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT p, uint32_t msg);

std::string Proxy_midiInGetErrorText(Proxy_MMRESULT);

constexpr Proxy_MMRESULT Proxy_MMSYSERR_NOERROR = 0;

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_PROXY_H_
