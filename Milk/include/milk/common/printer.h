#ifndef LIB_MILK_COMMON_PRINTER_H_
#define LIB_MILK_COMMON_PRINTER_H_

#include <tuple>

#include <milk/core.h>

namespace Milk {

class BasePrinter {
  using raw_type = Milk_Printer_t;
  using interface_type = Milk_Printer_Interface_t;
 public:
  virtual raw_type ToRawType() {
    return raw_type{
      .self_ = this,
      .interface_ = &Interface(),
    };
  }
  virtual void Print(const char [], size_t) = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace Milk

#endif // ifndef LIB_MILK_COMMON_PRINTER_H_
