#ifndef LIB_SOYBEAN_WINDOWS_WRAPPER_H_
#define LIB_SOYBEAN_WINDOWS_WRAPPER_H_

#include <soybean_windows/common.h>

#ifdef __cplusplus

namespace SoyBean_Windows {

inline
SoyBean_Factory_t make_factory(unsigned int uDeviceID, void *dwCallback, void *dwInstance, uint32_t fdwOpen) {
  SoyBean_Factory_t obj{};
  MilkTea_invoke_panic(SoyBean_Windows_Factory_Create, &obj, uDeviceID, dwCallback, dwInstance, fdwOpen);
  return obj;
}

} // namespace SoyBean_Windows

#endif // ifdef __cplusplus

#endif // ifndef LIB_SOYBEAN_WINDOWS_WRAPPER_H_
