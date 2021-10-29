#ifndef LIB_MILK_CORE_H_
#define LIB_MILK_CORE_H_

#include <milktea.h>
#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

MilkTea_API void MilkTea_CALL
Milk_Inject_Logger(MilkTea_Logger_t (MilkTea_CALL *inject)(MilkTea_LogLevel_t level));

MilkTea_API void MilkTea_CALL
Milk_Inject_SoyBean_Factory(MilkTea_Exception_t (MilkTea_CALL *inject)(SoyBean_Factory_t *factory));

MilkTea_API int MilkTea_CALL
Milk_Main(int argc, char *argv[]);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILK_CORE_H_
