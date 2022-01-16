#ifndef LIB_MILK_CONSOLE_WRAPPER_H_
#define LIB_MILK_CONSOLE_WRAPPER_H_

#include <milk_console/common.h>

#ifdef __cplusplus

namespace Milk_Console {

class ContextHolder final : public Milk::BaseContext {
  static constexpr char TAG[] = "Milk_Console::ContextHolder";
  using raw_type = Milk_Context_t;
  using interface_type = Milk_Context_Interface_t;
 public:
  ContextHolder(BaseFoundation &foundation) : self_({}) {
    MilkTea_invoke_panic(Milk_Console_Context_Create, &self_, foundation.ToRawType());
  }
  ContextHolder(ContextHolder &&another) : self_(another.release()) {}
  ~ContextHolder() {
    auto self = release();
    if (self.self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(Milk_Console_Context_Destroy, self);
  }
  raw_type ToRawType() final {
    return self_;
  }
  void SetLogLevel(MilkTea::Logger::Level) final {
    MilkTea_stub(SetLogLevel);
  }
  SoyBean::FactoryWrapper GetSoyBeanFactory() final {
    MilkTea_stub(GetSoyBeanFactory);
  }
  Milk::BasePrinter &GetPrinterOut() final {
    MilkTea_stub(GetPrinterOut);
  }
  Milk::BasePrinter &GetPrinterErr() final {
    MilkTea_stub(GetPrinterErr);
  }
  std::unique_ptr<Milk::BaseFileReader> GetFileReader(const char [], size_t) final {
    MilkTea_stub(GetFileReader);
  }
  std::unique_ptr<Milk::BaseFileWriter> GetFileWriter(const char [], size_t) final {
    MilkTea_stub(GetFileWriter);
  }
 private:
  raw_type release() {
    raw_type result = {};
    std::swap(self_, result);
    return result;
  }
  raw_type self_;
  MilkTea_NonCopy(ContextHolder)
  MilkTea_NonMoveAssign(ContextHolder)
};

} // namespace Milk_Console

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_CONSOLE_WRAPPER_H_
