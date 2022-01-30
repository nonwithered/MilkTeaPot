#ifndef MILK_CONSOLE_CONTEXT_H_
#define MILK_CONSOLE_CONTEXT_H_

#include <milk_console/common.h>

#include "printer.h"
#include "logger.h"
#include "fileio.h"
#include "foundation.h"

namespace Milk_Console {

class ContextImpl final : public Milk::BaseContext {
  static constexpr char TAG[] = "Milk_Console::ContextImpl";
 public:
  ContextImpl(FoundationWrapper foundation)
  : foundation_(foundation),
    out_(std::cout),
    err_(std::cerr) {}
  void SetLogLevel(MilkTea::Logger::Level level) final {
    bool b = MilkTea::Logger::Config<LoggerImpl>(LoggerImpl(level));
    if (!b) {
      MilkTea_logW("SetLogLevel fail");
    }
  }
  SoyBean::BaseFactory &GetSoyBeanFactory() final {
    return foundation_.GetSoyBeanFactory();
  }
  Milk::BasePrinter &GetPrinterOut() final {
    return out_;
  }
  Milk::BasePrinter &GetPrinterErr() final {
    return err_;
  }
  MilkTea::BaseReader &GetFileReader(std::string_view name) final {
    std::ifstream fs(name.data(), std::ios::binary);
    if (!fs.is_open()) {
      MilkTea_throwf(IOError, "GetFileReader fail -- %s", name.data());
    }
    return *new FileReaderImpl(std::move(fs));
  }
  MilkTea::BaseWriter &GetFileWriter(std::string_view name) final {
    std::ofstream fs(name.data(), std::ios::binary);
    if (!fs.is_open()) {
      MilkTea_throwf(IOError, "GetFileWriter fail -- %s", name.data());
    }
    return *new FileWriterImpl(std::move(fs));
  }
 private:
  FoundationWrapper foundation_;
  PrinterImpl out_;
  PrinterImpl err_;
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_CONTEXT_H_
