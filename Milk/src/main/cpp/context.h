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
  ContextWrapper(raw_type another = {})
  : obj_(another),
    out_([another]() -> auto {
      MilkTea_Writer_t result = {};
      MilkTea_invoke_panic(another.interface_->GetOutWriter, another.obj_, &result);
      return result;
    }()),
    err_([another]() -> auto {
      MilkTea_Writer_t result = {};
      MilkTea_invoke_panic(another.interface_->GetErrWriter, another.obj_, &result);
      return result;
    }()) {}
  void SetLogLevel(MilkTea::Logger::Level level) {
    MilkTea_invoke_panic(interface().SetLogLevel, obj(), MilkTea::Logger::ToRawType(level));
  }
  MilkTea::BaseWriter &GetOutWriter() {
    return out_;
  }
  MilkTea::BaseWriter &GetErrWriter() {
    return err_;
  }
  MilkTea::ReaderWrapper GetFileReader(std::string_view name) {
    MilkTea_Reader_t result = {};
    MilkTea_invoke_panic(interface().GetFileReader, obj(), &result, name.data(), name.size());
    return result;
  }
  MilkTea::WriterWrapper GetFileWriter(std::string_view name) {
    MilkTea_Writer_t result = {};
    MilkTea_invoke_panic(interface().GetFileWriter, obj(), &result, name.data(), name.size());
    return result;
  }
  SoyBean::FactoryWrapper GetSoyBeanFactory() {
    SoyBean_Factory_t result = {};
    MilkTea_invoke_panic(interface().GetSoyBeanFactory, obj(), &result);
    return result;
  }
 private:
  void *obj() const {
    return get().obj_;
  }
  const interface_type &interface() const {
    return *get().interface_;
  }
  const raw_type &get() const {
    return obj_;
  }
  const raw_type obj_;
  MilkTea::WriterWrapper out_;
  MilkTea::WriterWrapper err_;
};


} // namespace Milk

#endif // ifndef MILK_CONTEXT_H_
