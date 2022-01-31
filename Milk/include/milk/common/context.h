#ifndef LIB_MILK_COMMON_CONTEXT_H_
#define LIB_MILK_COMMON_CONTEXT_H_

#include <cstdlib>
#include <iostream>

#include <milk/core.h>

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
  virtual MilkTea_Writer_t GetOutWriter() = 0;
  virtual MilkTea_Writer_t GetErrWriter() = 0;
  virtual MilkTea_Reader_t GetFileReader(std::string_view) = 0;
  virtual MilkTea_Writer_t GetFileWriter(std::string_view) = 0;
  virtual SoyBean_Factory_t GetSoyBeanFactory() = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace Milk

#endif // ifndef LIB_MILK_COMMON_CONTEXT_H_
