#include <soybean/common.h>

namespace {

constexpr char TAG[] = "SoyBean::BaseFactory";

SoyBean::BaseFactory &BaseFactory_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<SoyBean::BaseFactory *>(self);
}

MilkTea_Exception_t MilkTea_call SoyBean_BaseFactory_Interface_Deleter(void *self) MilkTea_with_except({
  std::move(BaseFactory_cast(self)).Destroy();
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseFactory_Interface_Create(void *self, SoyBean_Handle_t *handle) MilkTea_with_except({
  MilkTea_nonnull(handle);
  *handle = BaseFactory_cast(self).Create();
})

} // namespace

namespace SoyBean {

const BaseFactory::interface_type &BaseFactory::Interface() {
  static const interface_type instance_{
    .Deleter = SoyBean_BaseFactory_Interface_Deleter,
    .Create = SoyBean_BaseFactory_Interface_Create,
  };
  return instance_;
}

} // namespace SoyBean
