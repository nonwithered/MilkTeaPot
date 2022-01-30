#ifndef MILK_CONSOLE_PRINTER_H_
#define MILK_CONSOLE_PRINTER_H_

#include <ostream>
#include <string_view>

#include <milk_console/common.h>

namespace Milk_Console {

class PrinterImpl final : public MilkTea::BaseWriter {
  static constexpr char TAG[] = "Milk_Console::PrinterImpl";
 public:
  explicit PrinterImpl(std::ostream &os) : os_(os) {}
  PrinterImpl(PrinterImpl &&another) : os_(another.os_) {}
  void Write(const uint8_t msg[], size_t len) final {
    MilkTea_nonnull(msg);
    const char *c = reinterpret_cast<const char *>(msg);
    std::string s;
    std::string sv;
    if (c[len] == '\0') {
      sv = std::string_view(c, len);
    } else {
      std::string s(c, len);
      sv = s;
    }
    Print(sv);
  }
  MilkTea::BaseWriter &Move() && final {
    return *new PrinterImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
 private:
  void Print(std::string_view s) {
    os_ << s << std::endl;
  }
  std::ostream &os_;
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_PRINTER_H_
