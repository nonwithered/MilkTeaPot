#ifndef LIB_MILK_WRAPPER_CONFIG_H_
#define LIB_MILK_WRAPPER_CONFIG_H_

#include <milk/common.h>

namespace Milk {

template<typename T>
class BaseConfig {
 protected:
  static MilkTea::LoggerWrapper make_logger(MilkTea::Logger::Level level);
  static SoyBean::FactoryWrapper make_soybean_factory();
};


template<typename T>
MilkTea_Logger_t MilkTea_call Make_MilkTea_Logger_(MilkTea_Logger_Level_t level) {
  return T::make_logger(MilkTea::Logger::FromRawType(level)).ToRawType();
}
template<typename T>
MilkTea_Exception_t MilkTea_call Make_SoyBean_Factory_(SoyBean_Factory_t *factory) MilkTea_with_except({
  *factory = T::make_soybean_factory().ToRawType();
})

template<typename T>
void Init() {
  MilkTea_invoke_panic(Milk_Init, Milk_Config_t{
    .Make_MilkTea_Logger_ = Make_MilkTea_Logger_<T>,
    .Make_SoyBean_Factory_ = Make_SoyBean_Factory_<T>,
  });
}

inline int Main(int argc, char *argv[]) {
  return Milk_Main(argc, argv);
}

} // namespace Milk

#endif // ifndef LIB_MILK_WRAPPER_CONFIG_H_
