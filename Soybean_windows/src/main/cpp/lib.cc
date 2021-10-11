#include <soybean_windows.h>

#include "factory_impl.h"

namespace {

constexpr char TAG[] = "SoyBean_Windows_API";

} // namespace

SoyBean::BaseFactory &SoyBean_Windows::FactoryImpl::Instance() {
  static SoyBean_Windows::FactoryImpl instance_;
  return instance_;
}

extern "C" {

SoyBean_Windows_Factory_t *
SoyBean_Windows_Factory_Instance() {
  return reinterpret_cast<SoyBean_Windows_Factory_t *>(&SoyBean_Windows::FactoryImpl::Instance());
}

MilkTea_IMPL(SoyBean_Windows_Factory_Callback, (void *factory, SoyBean_Handle_t *handle), {
  MilkTea_nonnull(factory);
  *handle = reinterpret_cast<SoyBean::BaseFactory *>(factory)->Create()->ToRawHandle();
})

} // extern "C"
