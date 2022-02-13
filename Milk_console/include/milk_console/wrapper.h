#ifndef LIB_MILK_CONSOLE_WRAPPER_H_
#define LIB_MILK_CONSOLE_WRAPPER_H_

#include <milk_console/common.h>

#ifdef __cplusplus

namespace Milk_Console {

inline
Milk_Context_t make_context(BaseFoundation &foundation) {
  Milk_Context_t context = {};
  MilkTea::Exception::Suppress([&]() {
    MilkTea_invoke_panic(Milk_Console_Context_Create, &context, foundation.ToRawType());
  });
  return context;
}

} // namespace Milk_Console

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_CONSOLE_WRAPPER_H_
