#ifndef MILK_CONFIG_H_
#define MILK_CONFIG_H_

#include <milk.h>

namespace Milk {

class ConfigWrapper {
 public:
  static ConfigWrapper &Instance(ConfigWrapper *instance = nullptr);
  static ConfigWrapper FromRawType(Milk_Config_t &&self) {
    ConfigWrapper self_{};
    std::swap(self_.self_, self);
    return self_;
  }
  MilkTea::LoggerWrapper Make_MilkTea_Logger(MilkTea::Logger::Level level) {
    auto Make_MilkTea_Logger_ = self_.Make_MilkTea_Logger_;
    MilkTea_nonnull(Make_MilkTea_Logger_);
    return MilkTea::LoggerWrapper::FromRawType(Make_MilkTea_Logger_(MilkTea::Logger::ToRawType(level)));
  }
  SoyBean::FactoryWrapper Make_SoyBean_Factory() {
    auto Make_SoyBean_Factory_ = self_.Make_SoyBean_Factory_;
    MilkTea_nonnull(Make_SoyBean_Factory_);
    SoyBean_Factory_t factory{};
    MilkTea_panic(Make_SoyBean_Factory_(&factory));
    return SoyBean::FactoryWrapper::FromRawType(std::move(factory));
  }
 private:
  Milk_Config_t self_;
  static constexpr char TAG[] = "Milk#Config";
};



} // namespace Milk

#endif // ifndef MILK_CONFIG_H_
