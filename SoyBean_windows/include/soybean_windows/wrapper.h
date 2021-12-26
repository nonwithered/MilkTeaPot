#ifndef LIB_SOYBEAN_WINDOWS_WRAPPER_H_
#define LIB_SOYBEAN_WINDOWS_WRAPPER_H_

#include <soybean_windows/common.h>

#ifdef __cplusplus

namespace SoyBean_Windows {

inline
SoyBean::FactoryWrapper make_factory(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen) {
  SoyBean_Factory_t self{};
  MilkTea_invoke_panic(SoyBean_Windows_Factory_Create, &self, uDeviceID, dwCallback, dwInstance, fdwOpen);
  return self;
}

} // namespace SoyBean_Windows

#endif // ifdef __cplusplus

#endif // ifndef LIB_SOYBEAN_WINDOWS_WRAPPER_H_
