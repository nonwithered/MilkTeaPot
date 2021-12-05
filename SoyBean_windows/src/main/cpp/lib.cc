#include <soybean_windows.h>

#include "factory.h"

namespace {

constexpr char TAG[] = "SoyBean_Windows";

} // namespace

MilkTea_extern(SoyBean_Windows_Factory_Create, (SoyBean_Factory_t *self, unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen), {
  MilkTea_nonnull(self);
  *self = SoyBean_Windows::FactoryImpl(uDeviceID, dwCallback, dwInstance, fdwOpen).ToRawType();
})
