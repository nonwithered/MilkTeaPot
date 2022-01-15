#include <milk/common.h>

namespace {

constexpr char TAG[] = "Milk#Printer";

Milk::BasePrinter &BasePrinter_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<Milk::BasePrinter *>(self);
}

MilkTea_Exception_t MilkTea_call Milk_Printer_Interface_Print(void *self, const char msg[], size_t len) MilkTea_with_except({
  BasePrinter_cast(self).Print(msg, len);
})

} // namespace

namespace Milk {

const BasePrinter::interface_type &BasePrinter::Interface() {
  static const interface_type instance_{
    .Print = Milk_Printer_Interface_Print,
  };
  return instance_;
}

} // namespace Milk
