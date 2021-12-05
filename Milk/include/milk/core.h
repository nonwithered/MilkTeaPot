#ifndef LIB_MILK_CORE_H_
#define LIB_MILK_CORE_H_

#include <milktea.h>
#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Milk_Config_t {
  MilkTea_Logger_t (MilkTea_call *Make_MilkTea_Logger_)(MilkTea_Logger_Level_t level);
  MilkTea_Exception_t (MilkTea_call *Make_SoyBean_Factory_)(SoyBean_Factory_t *factory);
};

#ifndef __cplusplus
typedef struct Milk_Config_t Milk_Config_t;
#endif

MilkTea_api
Milk_Init(Milk_Config_t);

MilkTea_decl(int)
Milk_Main(int argc, char *argv[]);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILK_CORE_H_
