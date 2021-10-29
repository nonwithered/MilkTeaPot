#include <milk.h>

#include <soybean_windows.h>

namespace {

MilkTea_Exception_t MilkTea_CALL Milk_Inject_SoyBean_Factory_impl(SoyBean_Factory_t *factory) {
  return SoyBean_Windows_Factory_Create(factory, 0, nullptr, nullptr, 0);
}

} // namespace

int main(int argc, char *argv[]) {
  Milk_Inject_SoyBean_Factory(Milk_Inject_SoyBean_Factory_impl);
  return Milk_Main(argc, argv);
}
