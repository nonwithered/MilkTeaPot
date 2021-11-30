#include <milk.h>

#include <soybean_windows.h>

namespace {

MilkTea_Logger_t MilkTea_call Milk_Config_Make_MilkTea_Logger(MilkTea_Logger_Level_t level) {
  return Milk::LoggerImpl(MilkTea::Logger::FromRawType(level)).ToRawType();
}

MilkTea_Exception_t MilkTea_call Milk_Config_Make_SoyBean_Factory(SoyBean_Factory_t *factory) {
  return SoyBean_Windows_Factory_Create(factory, 0, nullptr, nullptr, 0);
}

} // namespace

int main(int argc, char *argv[]) {
  Milk::Init(Milk_Config_t{
    .Make_MilkTea_Logger_ = Milk_Config_Make_MilkTea_Logger,
    .Make_SoyBean_Factory_ = Milk_Config_Make_SoyBean_Factory,
  });
  return Milk::Main(argc, argv);
}
