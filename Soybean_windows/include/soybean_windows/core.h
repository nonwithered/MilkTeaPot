#ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
#define LIB_SOYBEAN_WINDOWS_CORE_H_

#include <milktea.h>

#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyBean_Windows_Factory_t;

#ifndef __cplusplus
typedef struct SoyBean_Windows_Factory_t SoyBean_Windows_Factory_t;
#endif

MilkTea_API SoyBean_Windows_Factory_t * MilkTea_CALL
SoyBean_Windows_Factory_Instance();

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyBean_Windows_Factory_Callback(void *factory, SoyBean_Handle_t *handle);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYBEAN_WINDOWS_CORE_H_
