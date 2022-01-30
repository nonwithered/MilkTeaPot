#ifndef LIB_MILK_CORE_H_
#define LIB_MILK_CORE_H_

#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Milk_Context_Interface_t {
  MilkTea_Exception_t (MilkTea_call *SetLogLevel)(void *self, MilkTea_Logger_Level_t level);
  MilkTea_Exception_t (MilkTea_call *GetSoyBeanFactory)(void *self, SoyBean_Factory_t *factory);
  MilkTea_Exception_t (MilkTea_call *GetOutWriter)(void *self, MilkTea_Writer_t *printer);
  MilkTea_Exception_t (MilkTea_call *GetErrWriter)(void *self, MilkTea_Writer_t *printer);
  MilkTea_Exception_t (MilkTea_call *GetFileReader)(void *self, MilkTea_Reader_t *reader, const char name[], size_t len);
  MilkTea_Exception_t (MilkTea_call *GetFileWriter)(void *self, MilkTea_Writer_t *writer, const char name[], size_t len);
};

#ifndef __cplusplus
typedef struct Milk_Context_Interface_t Milk_Context_Interface_t;
#endif

struct Milk_Context_t {
  void *self_;
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
