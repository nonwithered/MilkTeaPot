#include <soybean_windows.h>

#include "factory_impl.h"

namespace {

constexpr char TAG[] = "SoyBean_Windows_API";

MilkTea_Exception_t MilkTea_CALL SoyBean_Factory_Interface_Deletor(void *self) MilkTea_with_except({
  MilkTea_nonnull(self);
  delete reinterpret_cast<SoyBean_Windows::FactoryImpl *>(self);
})

MilkTea_Exception_t MilkTea_CALL SoyBean_Factory_Interface_Create(void *self, SoyBean_Handle_t *handle) MilkTea_with_except({
  MilkTea_nonnull(self);
  *handle = reinterpret_cast<SoyBean_Windows::FactoryImpl *>(self)->Create()->ToRawHandle();
})

SoyBean_Factory_Interface_t &SoyBean_Factory_Interface_Instance() {
  static SoyBean_Factory_Interface_t interface_{
    .Deletor = SoyBean_Factory_Interface_Deletor,
    .Create = SoyBean_Factory_Interface_Create,
  };
  return interface_;
}

} // namespace

extern "C" {

MilkTea_IMPL(SoyBean_Windows_Factory_Create, (SoyBean_Factory_t *self, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen), {
  MilkTea_nonnull(self);
  self->factory_ = reinterpret_cast<void *>(new SoyBean_Windows::FactoryImpl(uDeviceID, dwCallback, dwInstance, fdwOpen));
  self->interface_ = &SoyBean_Factory_Interface_Instance();
})

} // extern "C"
