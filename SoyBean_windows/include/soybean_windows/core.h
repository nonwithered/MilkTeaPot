#ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
#define LIB_SOYBEAN_WINDOWS_CORE_H_

#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

MilkTea_api
SoyBean_Windows_Factory_Create(SoyBean_Factory_t *obj, unsigned int uDeviceID, void *dwCallback, void *dwInstance, uint32_t fdwOpen);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
