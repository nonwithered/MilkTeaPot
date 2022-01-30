#ifndef LIB_MILK_CONSOLE_COMMON_H_
#define LIB_MILK_CONSOLE_COMMON_H_

#include <milk_console/core.h>

#ifdef __cplusplus

namespace Milk_Console {

class BaseFoundation {
  using raw_type = Milk_Console_Foundation_t;
  using interface_type = Milk_Console_Foundation_Interface_t;
 public:
  virtual raw_type ToRawType() {
    return raw_type{
      .self_ = this,
      .interface_ = &Interface(),
    };
  }
  virtual SoyBean::BaseFactory &GetSoyBeanFactory() = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace Milk_Console

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_CONSOLE_COMMON_H_
