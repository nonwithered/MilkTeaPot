#ifndef MILK_CONFIG_H_
#define MILK_CONFIG_H_

#include <milk.h>

namespace Milk {

class ConfigWrapper {
 public:
  static ConfigWrapper &Instance(ConfigWrapper *instance = nullptr);
  ConfigWrapper(Milk_Config_t another) : self_(another) {}
  MilkTea::LoggerWrapper make_logger(MilkTea::Logger::Level level) {
    auto Make_MilkTea_Logger_ = self_.Make_MilkTea_Logger_;
    MilkTea_nonnull(Make_MilkTea_Logger_);
    return Make_MilkTea_Logger_(MilkTea::Logger::ToRawType(level));
  }
  SoyBean::FactoryWrapper make_soybean_factory() {
    auto Make_SoyBean_Factory_ = self_.Make_SoyBean_Factory_;
    MilkTea_nonnull(Make_SoyBean_Factory_);
    SoyBean_Factory_t factory{};
    MilkTea_panic(Make_SoyBean_Factory_(&factory));
    return factory;
  }
 private:
  Milk_Config_t self_;
  static constexpr char TAG[] = "Milk#Config";
};



} // namespace Milk

#endif // ifndef MILK_CONFIG_H_
