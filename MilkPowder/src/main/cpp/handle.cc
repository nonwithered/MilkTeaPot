#include <milkpowder/common.h>

namespace {

constexpr char TAG[] = "SoyBean::BaseHandle";

SoyBean::BaseHandle &BaseHandle_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<SoyBean::BaseHandle *>(obj);
}

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_Deleter(void *obj) MilkTea_with_except({
  std::move(BaseHandle_cast(obj)).Destroy();
})

MilkTea_Exception_t MilkTea_call SoyBean_BaseHandle_Interface_SendMessage(void *obj, uint8_t type, uint8_t arg0, uint8_t arg1) MilkTea_with_except({
  BaseHandle_cast(obj).SendMessage(type, arg0, arg1);
})

} // namespace

namespace SoyBean {

const BaseHandle::interface_type &BaseHandle::Interface() {
  static const interface_type instance_{
    .Deleter = SoyBean_BaseHandle_Interface_Deleter,
    .SendMessage = SoyBean_BaseHandle_Interface_SendMessage,
  };
  return instance_;
}

} // namespace SoyBean
