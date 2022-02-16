#ifndef LIB_MILK_CORE_H_
#define LIB_MILK_CORE_H_

#include <milkpowder.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Milk_Context_Interface_t {
  MilkTea_Exception_t (MilkTea_call *SetLogLevel)(void *obj, MilkTea_Logger_Level_t level);
  MilkTea_Exception_t (MilkTea_call *GetSoyBeanFactory)(void *obj, SoyBean_Factory_t *factory);
  MilkTea_Exception_t (MilkTea_call *GetOutWriter)(void *obj, MilkTea_Writer_t *printer);
  MilkTea_Exception_t (MilkTea_call *GetErrWriter)(void *obj, MilkTea_Writer_t *printer);
  MilkTea_Exception_t (MilkTea_call *GetFileReader)(void *obj, MilkTea_Reader_t *reader, const char name[], size_t len);
  MilkTea_Exception_t (MilkTea_call *GetFileWriter)(void *obj, MilkTea_Writer_t *writer, const char name[], size_t len);
};

#ifndef __cplusplus
typedef struct Milk_Context_Interface_t Milk_Context_Interface_t;
#endif

struct Milk_Context_t {
  void *obj_;
  const Milk_Context_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct Milk_Context_t Milk_Context_t;
#endif

MilkTea_api
Milk_Main(int argc, char *argv[], Milk_Context_t context);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILK_CORE_H_
