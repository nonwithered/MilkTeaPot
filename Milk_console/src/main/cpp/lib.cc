#include <milk_console/common.h>

#include "context.h"

namespace {

constexpr char TAG[] = "Milk_console";

Milk_Console::BaseFoundation &BaseFoundation_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<Milk_Console::BaseFoundation *>(obj);
}

Milk_Console::ContextImpl &ContextImpl_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<Milk_Console::ContextImpl *>(obj);
}

MilkTea_Exception_t MilkTea_call Milk_Console_Foundation_Interface_GetSoyBeanFactory(void *obj, SoyBean_Factory_t *factory) MilkTea_with_except({
  MilkTea_nonnull(factory);
  *factory = BaseFoundation_cast(obj).GetSoyBeanFactory();
})

} // namespace

MilkTea_extern(Milk_Console_Context_Create, (Milk_Context_t *obj, Milk_Console_Foundation_t foundation), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(foundation.interface_);
  *obj = (new Milk_Console::ContextImpl(Milk_Console::FoundationWrapper(foundation)))->ToRawType();
})

MilkTea_extern(Milk_Console_Context_Destroy, (Milk_Context_t obj), {
  MilkTea_nonnull(obj.obj_);
  delete &ContextImpl_cast(obj.obj_);
})

namespace Milk_Console {

const BaseFoundation::interface_type &BaseFoundation::Interface() {
  static const interface_type instance_{
    .GetSoyBeanFactory = Milk_Console_Foundation_Interface_GetSoyBeanFactory,
  };
  return instance_;
}

} // namespace Milk_Console
