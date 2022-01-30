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
  void Write(const uint8_t bytes[], size_t len) final {
    MilkTea_nonnull(bytes);
    os_.write(reinterpret_cast<const char *>(bytes), len);
  }
  MilkTea::BaseWriter &Move() && final {
    return *new PrinterImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
 private:
  std::ostream &os_;
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_PRINTER_H_
