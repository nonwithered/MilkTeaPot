#ifndef MILK_CONTEXT_H_
#define MILK_CONTEXT_H_

#include <milk/common.h>

#include "fileio.h"
#include "printer.h"

namespace Milk {

class ContextWrapper final : public BaseContext {
  static constexpr char TAG[] = "Milk::ContextWrapper";
  using raw_type = Milk_Context_t;
  using interface_type = Milk_Context_Interface_t;
 public:
  ContextWrapper(raw_type another = {}) : self_(another), out_(), err_() {
    if (self() == nullptr) {
      return;
    }
    Milk_Printer_t out = {};
    MilkTea_invoke_panic(interface().GetPrinterOut, self(), &out);
    out_ = out;
    Milk_Printer_t err = {};
    MilkTea_invoke_panic(interface().GetPrinterErr, self(), &err);
    err_ = err;
  }
  raw_type ToRawType() final {
    return self_;
  }
  void SetLogLevel(MilkTea::Logger::Level level) final {
    MilkTea_invoke_panic(interface().SetLogLevel, self(), MilkTea::Logger::ToRawType(level));
  }
  std::unique_ptr<SoyBean::BaseFactory> GetSoyBeanFactory() final {
    SoyBean_Factory_t result = {};
    MilkTea_invoke_panic(interface().GetSoyBeanFactory, self(), &result);
    return std::make_unique<SoyBean::FactoryWrapper>(result);
  }
  BasePrinter &GetPrinterOut() final {
    return out_;
  }
  BasePrinter &GetPrinterErr() final {
    return err_;
  }
  std::unique_ptr<BaseFileReader> GetFileReader(const char name[], size_t len) final {
    Milk_FileReader_t result = {};
    MilkTea_invoke_panic(interface().GetFileReader, self(), &result, name, len);
    return std::make_unique<FileReaderWrapper>(result);
  }
  std::unique_ptr<BaseFileWriter> GetFileWriter(const char name[], size_t len) final {
    Milk_FileWriter_t result = {};
    MilkTea_invoke_panic(interface().GetFileWriter, self(), &result, name, len);
    return std::make_unique<FileWriterWrapper>(result);
  }
 private:
  void *self() const {
    return get().self_;
  }
  const interface_type &interface() const {
    return *get().interface_;
  }
  const raw_type &get() const {
    return self_;
  }
  const raw_type self_;
  PrinterWripper out_;
  PrinterWripper err_;
};


} // namespace Milk

#endif // ifndef MILK_CONTEXT_H_
