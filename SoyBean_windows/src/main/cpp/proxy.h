#ifndef SOYBEAN_WINDOWS_PROXY_H_
#define SOYBEAN_WINDOWS_PROXY_H_

#include <string>
#include <cstdint>

namespace SoyBean_Windows {

struct Proxy_HMIDIOUT_t;
using Proxy_HMIDIOUT = Proxy_HMIDIOUT_t *;
using Proxy_MMRESULT = unsigned int;

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT &hmo, unsigned int uDeviceID, void *dwCallback, void *dwInstance, uint32_t fdwOpen);
Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT hmo);
Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT hmo, uint32_t dwMsg);

std::string Proxy_midiInGetErrorText(Proxy_MMRESULT mmrError);

constexpr Proxy_MMRESULT Proxy_MMSYSERR_NOERROR = 0;

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_PROXY_H_
