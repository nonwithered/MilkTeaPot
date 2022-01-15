#include <milk_console/common.h>

#include "context.h"

namespace {

constexpr char TAG[] = "Milk_console";

Milk_Console::BaseFoundation &BaseFoundation_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<Milk_Console::BaseFoundation *>(self);
}

Milk_Console::ContextImpl &ContextImpl_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<Milk_Console::ContextImpl *>(self);
}

MilkTea_Exception_t MilkTea_call Milk_Console_Foundation_Interface_GetSoyBeanFactory(void *self, SoyBean_Factory_t *factory) MilkTea_with_except({
  MilkTea_nonnull(factory);
  *factory = BaseFoundation_cast(self).GetSoyBeanFactory().ToRawType();
})

} // namespace

MilkTea_extern(Milk_Console_Context_Create, (Milk_Context_t *self, Milk_Console_Foundation_t foundation), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(foundation.interface_);
  *self = (new Milk_Console::ContextImpl(Milk_Console::FoundationWrapper(foundation)))->ToRawType();
})

MilkTea_extern(Milk_Console_Context_Destroy, (Milk_Context_t self), {
  MilkTea_nonnull(self.self_);
  delete &ContextImpl_cast(self.self_);
})

namespace Milk_Console {

const BaseFoundation::interface_type &BaseFoundation::Interface() {
  static const interface_type instance_{
    .GetSoyBeanFactory = Milk_Console_Foundation_Interface_GetSoyBeanFactory,
  };
  return instance_;
}

} // namespace Milk_Console
