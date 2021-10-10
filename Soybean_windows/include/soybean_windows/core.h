#ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
#define LIB_SOYBEAN_WINDOWS_CORE_H_

#include <milktea.h>

#define SoyBean_Windows_API MilkTea_API

#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

SoyBean_Windows_API const char *
SoyBean_Windows_Exception_What();

struct SoyBean_Windows_Factory_t;

#ifndef __cplusplus
typedef struct SoyBean_Windows_Factory_t SoyBean_Windows_Factory_t;
#endif

SoyBean_Windows_API SoyBean_Windows_Factory_t *
SoyBean_Windows_Factory_Instance();

SoyBean_Windows_API MilkTea_Exception_t
(*SoyBean_Windows_Factory_Callback())(void *factory, SoyBean_Handle_t *handle);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
