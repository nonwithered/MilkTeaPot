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
    bool b = MilkTea::Logger::Config(LoggerImpl(level));
    if (!b) {
      MilkTea_logW("SetLogLevel fail");
    }
  }
  SoyBean::FactoryWrapper GetSoyBeanFactory() final {
    return foundation_.GetSoyBeanFactory();
  }
  Milk::BasePrinter &GetPrinterOut() final {
    return out_;
  }
  Milk::BasePrinter &GetPrinterErr() final {
    return err_;
  }
  std::unique_ptr<Milk::BaseFileReader> GetFileReader(const char name[], size_t len) final {
    std::string_view v;
    std::string s;
    if (name[len] == '\0') {
      v = name;
    } else {
      s = std::string(name, name + len);
      v = s;
    }
    std::ifstream fs(v.data(), std::ios::binary);
    if (!fs.is_open()) {
      MilkTea_throwf(IOError, "GetFileReader fail -- %s", v.data());
    }
    return std::make_unique<FileReaderImpl>(std::move(fs));
  }
  std::unique_ptr<Milk::BaseFileWriter> GetFileWriter(const char name[], size_t len) final {
    std::string_view v;
    std::string s;
    if (name[len] == '\0') {
      v = name;
    } else {
      s = std::string(name, name + len);
      v = s;
    }
    std::ofstream fs(v.data(), std::ios::binary);
    if (!fs.is_open()) {
      MilkTea_throwf(IOError, "GetFileWriter fail -- %s", v.data());
    }
    return std::make_unique<FileWriterImpl>(std::move(fs));
  }
 private:
  FoundationWrapper foundation_;
  PrinterImpl out_;
  PrinterImpl err_;
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_CONTEXT_H_
