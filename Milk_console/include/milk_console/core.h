#ifndef LIB_MILK_CONSOLE_CORE_H_
#define LIB_MILK_CONSOLE_CORE_H_

#include <milk.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Milk_Console_Foundation_Interface_t {
  MilkTea_Exception_t (MilkTea_call *GetSoyBeanFactory)(void *self, SoyBean_Factory_t *factory);
};

#ifndef __cplusplus
typedef struct Milk_Console_Foundation_Interface_t Milk_Console_Foundation_Interface_t;
#endif

struct Milk_Console_Foundation_t {
  void *self_;
  const Milk_Console_Foundation_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct Milk_Console_Foundation_t Milk_Console_Foundation_t;
#endif

MilkTea_api
Milk_Console_Context_Create(Milk_Context_t *self, Milk_Console_Foundation_t foundation);

MilkTea_api
Milk_Console_Context_Destroy(Milk_Context_t self);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILK_CONSOLE_CORE_H_
