#ifndef LIB_MILK_COMMON_CONTEXT_H_
#define LIB_MILK_COMMON_CONTEXT_H_

#include <cstdlib>
#include <iostream>

#include <milk/core.h>
#include <milk/common/fileio.h>
#include <milk/common/printer.h>

namespace Milk {

class BaseContext {
  using raw_type = Milk_Context_t;
  using interface_type = Milk_Context_Interface_t;
 public:
  virtual raw_type ToRawType() {
    return raw_type{
      .self_ = this,
      .interface_ = &Interface(),
    };
  }
  virtual void SetLogLevel(MilkTea::Logger::Level) = 0;
  virtual SoyBean::FactoryWrapper GetSoyBeanFactory() = 0;
  virtual BasePrinter &GetPrinterOut() = 0;
  virtual BasePrinter &GetPrinterErr() = 0;
  virtual std::unique_ptr<BaseFileReader> GetFileReader(const char [], size_t) = 0;
  virtual std::unique_ptr<BaseFileWriter> GetFileWriter(const char [], size_t) = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace Milk

#endif // ifndef LIB_MILK_COMMON_CONTEXT_H_
