#ifndef MILK_CONTEXT_H_
#define MILK_CONTEXT_H_

#include <milk/common.h>

#include "printer.h"

namespace Milk {

class ContextWrapper final {
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
  void SetLogLevel(MilkTea::Logger::Level level) {
    MilkTea_invoke_panic(interface().SetLogLevel, self(), MilkTea::Logger::ToRawType(level));
  }
  SoyBean::FactoryWrapper GetSoyBeanFactory() {
    SoyBean_Factory_t result = {};
    MilkTea_invoke_panic(interface().GetSoyBeanFactory, self(), &result);
    return result;
  }
  BasePrinter &GetPrinterOut() {
    return out_;
  }
  BasePrinter &GetPrinterErr() {
    return err_;
  }
  MilkTea::ReaderWrapper GetFileReader(std::string_view name) {
    MilkTea_Reader_t result = {};
    MilkTea_invoke_panic(interface().GetFileReader, self(), &result, name.data(), name.size());
    return result;
  }
  MilkTea::WriterWrapper GetFileWriter(std::string_view name) {
    MilkTea_Writer_t result = {};
    MilkTea_invoke_panic(interface().GetFileWriter, self(), &result, name.data(), name.size());
    return result;
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
