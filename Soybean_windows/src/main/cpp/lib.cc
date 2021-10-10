#include <soybean_windows.h>

#include "util.h"

MilkTea_Exception_What_IMPL(SoyBean_Windows)

extern "C"
SoyBean_Windows_API const char *
SoyBean_Windows_Exception_What() {
  return SoyBean_Windows::MilkTea_Exception_What();
}

#include "factory_impl.h"

namespace {

constexpr char TAG[] = "SoyBean_Windows_API";

} // namespace

SoyBean::BaseFactory &SoyBean_Windows::FactoryImpl::Instance() {
  static SoyBean_Windows::FactoryImpl instance_;
  return instance_;
}

extern "C" {

SoyBean_Windows_API SoyBean_Windows_Factory_t *
SoyBean_Windows_Factory_Instance() {
  return reinterpret_cast<SoyBean_Windows_Factory_t *>(&SoyBean_Windows::FactoryImpl::Instance());
}

SoyBean_Windows_API MilkTea_Exception_t
(*SoyBean_Windows_Factory_Callback())(void *factory, SoyBean_Handle_t *handle) {
  return SoyBean::BaseFactory::Callback();
}

} // extern "C"
