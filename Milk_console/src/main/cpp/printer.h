#ifndef MILK_CONSOLE_PRINTER_H_
#define MILK_CONSOLE_PRINTER_H_

#include <ostream>
#include <string_view>

#include <milk_console/common.h>

namespace Milk_Console {

class PrinterImpl final : public Milk::BasePrinter {
  static constexpr char TAG[] = "Milk_Console::PrinterImpl";
 public:
  explicit PrinterImpl(std::ostream &printer) : printer_(printer) {}
  void Print(const char msg[], size_t len) final {
    MilkTea_nonnull(msg);
    if (msg[len] == '\0') {
      Print(msg);
      return;
    }
    std::string s(msg, msg + len);
    Print(s);
  }
 private:
  void Print(std::string_view s) {
    printer_ << s << std::endl;
  }
  std::ostream &printer_;
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_PRINTER_H_
