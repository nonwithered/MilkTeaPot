#ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
#define LIB_SOYBEAN_WINDOWS_CORE_H_

#include <milktea.h>

#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyBean_Windows_Factory_Create(SoyBean_Factory_t *self, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
