#include <milkpowder/common.h>

namespace {

constexpr char TAG[] = "SoyBean";

} // namespace

MilkTea_extern(SoyBean_Handle_Create, (SoyBean_Handle_t *obj, SoyBean_Factory_t factory), {
  MilkTea_nonnull(obj);
  factory.interface_->Create(factory.obj_, obj);
})

MilkTea_extern(SoyBean_Handle_Destroy, (SoyBean_Handle_t obj), {
  return obj.interface_->Deleter(obj.obj_);
})

MilkTea_extern(SoyBean_Handle_SendMessage, (SoyBean_Handle_t obj, uint8_t type, uint8_t arg0, uint8_t arg1), {
  return obj.interface_->SendMessage(obj.obj_, type, arg0, arg1);
})

MilkTea_extern(SoyBean_Factory_Destroy, (SoyBean_Factory_t obj), {
  obj.interface_->Deleter(obj.obj_);
})
