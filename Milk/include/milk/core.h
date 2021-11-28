#ifndef LIB_MILK_CORE_H_
#define LIB_MILK_CORE_H_

#include <milktea.h>
#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Milk_Config_t {
  MilkTea_Logger_t (MilkTea_CALL *Make_MilkTea_Logger_)(MilkTea_Logger_Level_t level);
  MilkTea_Exception_t (MilkTea_CALL *Make_SoyBean_Factory_)(SoyBean_Factory_t *factory);
};

#ifndef __cplusplus
typedef struct Milk_Config_t Milk_Config_t;
#endif

MilkTea_API MilkTea_Exception_t MilkTea_CALL
Milk_Init(Milk_Config_t);

MilkTea_API int MilkTea_CALL
Milk_Main(int argc, char *argv[]);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
namespace Milk {

inline void Init(Milk_Config_t config) {
  MilkTea_panic(Milk_Init(config));
}

inline int Main(int argc, char *argv[]) {
  return Milk_Main(argc, argv);
}

} // namespace Milk
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_CORE_H_
