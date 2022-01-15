#ifndef LIB_MILK_WRAPPER_H_
#define LIB_MILK_WRAPPER_H_

#include <milk/common.h>

#ifdef __cplusplus

namespace Milk {

int Main(int argc, char *argv[], BaseContext &context) {
  return Milk_Main(argc, argv, context.ToRawType());
}

} // namespace Milk

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_WRAPPER_H_
