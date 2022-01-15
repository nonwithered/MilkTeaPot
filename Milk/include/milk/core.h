#ifndef LIB_MILK_CORE_H_
#define LIB_MILK_CORE_H_

#include <soybean.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Milk_Printer_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Print)(void *self, const char msg[], size_t len);
};

#ifndef __cplusplus
typedef struct Milk_Printer_Interface_t Milk_Printer_Interface_t;
#endif

struct Milk_Printer_t {
  void *self_;
  const Milk_Printer_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct Milk_Printer_t Milk_Printer_t;
#endif

struct Milk_FileReader_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Destroy)(void *self);
  MilkTea_Exception_t (MilkTea_call *Read)(void *self, uint8_t bytes[], size_t *len);
};

#ifndef __cplusplus
typedef struct Milk_FileReader_Interface_t Milk_FileReader_Interface_t;
#endif

struct Milk_FileReader_t {
  void *self_;
  const Milk_FileReader_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct Milk_FileReader_t Milk_FileReader_t;
#endif

struct Milk_FileWriter_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Destroy)(void *self);
  MilkTea_Exception_t (MilkTea_call *Write)(void *self, const uint8_t bytes[], size_t len);
};

#ifndef __cplusplus
typedef struct Milk_FileWriter_Interface_t Milk_FileWriter_Interface_t;
#endif

struct Milk_FileWriter_t {
  void *self_;
  const Milk_FileWriter_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct Milk_FileWriter_t Milk_FileWriter_t;
#endif

struct Milk_Context_Interface_t {
  MilkTea_Exception_t (MilkTea_call *SetLogLevel)(void *self, MilkTea_Logger_Level_t level);
  MilkTea_Exception_t (MilkTea_call *GetSoyBeanFactory)(void *self, SoyBean_Factory_t *factory);
  MilkTea_Exception_t (MilkTea_call *GetPrinterOut)(void *self, Milk_Printer_t *printer);
  MilkTea_Exception_t (MilkTea_call *GetPrinterErr)(void *self, Milk_Printer_t *printer);
  MilkTea_Exception_t (MilkTea_call *GetFileReader)(void *self, Milk_FileReader_t *reader, const char name[], size_t len);
  MilkTea_Exception_t (MilkTea_call *GetFileWriter)(void *self, Milk_FileWriter_t *writer, const char name[], size_t len);
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

MilkTea_decl(int)
Milk_Main(int argc, char *argv[], Milk_Context_t context);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILK_CORE_H_
