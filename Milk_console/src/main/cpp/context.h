#ifndef MILK_CONSOLE_CONTEXT_H_
#define MILK_CONSOLE_CONTEXT_H_

#include <milk_console/common.h>

#include "printer.h"
#include "logger.h"
#include "foundation.h"
#include "fileio.h"

namespace Milk_Console {

class ContextImpl final : public Milk::BaseContext {
  static constexpr char TAG[] = "Milk_Console::ContextImpl";
 public:
  ContextImpl(FoundationWrapper foundation)
  : foundation_(std::move(foundation)) {}
  void SetLogLevel(MilkTea::Logger::Level level) final {
    bool b = MilkTea::Logger::Config<LoggerImpl>(LoggerImpl(level));
    if (!b) {
      MilkTea_logW("SetLogLevel fail");
    }
  }
  SoyBean::BaseFactory &GetSoyBeanFactory() final {
    return foundation_.GetSoyBeanFactory();
  }
  MilkTea::BaseWriter &GetOutWriter() final {
    return *new PrinterImpl(std::cout);
  }
  MilkTea::BaseWriter &GetErrWriter() final {
    return *new PrinterImpl(std::cerr);
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
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_CONTEXT_H_
