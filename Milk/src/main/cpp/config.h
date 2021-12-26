#ifndef MILK_CONFIG_H_
#define MILK_CONFIG_H_

#include <milk.h>

namespace Milk {

class ConfigWrapper {
  static constexpr char TAG[] = "Milk::ConfigWrapper";
 public:
  static ConfigWrapper &Instance(ConfigWrapper *instance = nullptr);
  ConfigWrapper(Milk_Config_t another) : self_(another) {}
  MilkTea::LoggerWrapper make_logger(MilkTea::Logger::Level level) {
    MilkTea_nonnull(self_.Make_MilkTea_Logger_);
    return self_.Make_MilkTea_Logger_(MilkTea::Logger::ToRawType(level));
  }
  SoyBean::FactoryWrapper make_soybean_factory() {
    MilkTea_nonnull(self_.Make_SoyBean_Factory_);
    SoyBean_Factory_t factory{};
    MilkTea_invoke_panic(self_.Make_SoyBean_Factory_, &factory);
    return factory;
  }
 private:
  Milk_Config_t self_;
};



} // namespace Milk

#endif // ifndef MILK_CONFIG_H_
