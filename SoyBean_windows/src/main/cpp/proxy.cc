#include "proxy.h"

#include <windows.h>

#include <string>
#include <sstream>
#include <cstdint>

namespace SoyBean_Windows {

Proxy_MMRESULT Proxy_midiOutOpen(Proxy_HMIDIOUT &hmo, unsigned int uDeviceID, void *dwCallback, void *dwInstance, uint32_t fdwOpen) {
  return midiOutOpen((LPHMIDIOUT) &hmo, (UINT) uDeviceID, (DWORD_PTR) dwCallback, (DWORD_PTR) dwInstance, (DWORD) fdwOpen);
}

Proxy_MMRESULT Proxy_midiOutClose(Proxy_HMIDIOUT hmo) {
  return midiOutClose((HMIDIOUT) hmo);
}

Proxy_MMRESULT Proxy_midiOutShortMsg(Proxy_HMIDIOUT hmo, uint32_t dwMsg) {
  return midiOutShortMsg((HMIDIOUT) hmo, dwMsg);
}

std::string Proxy_midiInGetErrorText(Proxy_MMRESULT mmrError) {
  std::stringstream ss;
  ss << "MMRESULT";
  ss << " ";
  ss << mmrError;
  uint32_t cchText = 128;
  std::string pszText(cchText, 0);
  auto r = midiInGetErrorText(mmrError, pszText.data(), cchText);
  if (r == MMSYSERR_NOERROR) {
    ss << " ";
    ss << pszText;
  }
  return ss.str();
}

} // namespace SoyBean_Windows
