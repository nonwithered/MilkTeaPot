#ifndef LIB_MILK_WRAPPER_CONFIG_H_
#define LIB_MILK_WRAPPER_CONFIG_H_

#include <milk/common.h>

namespace Milk {

template<typename T>
class BaseConfig {
 protected:
  static MilkTea::LoggerWrapper MakeMilkTeaLogger(MilkTea::Logger::Level level);
  static SoyBean::FactoryWrapper MakeSoyBeanFactory();
 public:
  static Milk_Config_t ToRawType() {
    return Milk_Config_t{
      .Make_MilkTea_Logger_ = BaseConfig<T>::Make_MilkTea_Logger_,
      .Make_SoyBean_Factory_ = BaseConfig<T>::Make_SoyBean_Factory_,
    };
  }
 private:
  static MilkTea_Logger_t MilkTea_call Make_MilkTea_Logger_(MilkTea_Logger_Level_t level) {
    return T::MakeMilkTeaLogger(MilkTea::Logger::FromRawType(level)).ToRawType();
  }
  static MilkTea_Exception_t MilkTea_call Make_SoyBean_Factory_(SoyBean_Factory_t *factory) MilkTea_with_except({
    *factory = T::MakeSoyBeanFactory().ToRawType();
  })
};

template<typename T>
void Init() {
  MilkTea_panic(Milk_Init(BaseConfig<T>::ToRawType()));
}

inline int Main(int argc, char *argv[]) {
  return Milk_Main(argc, argv);
}

} // namespace Milk

#endif // ifndef LIB_MILK_WRAPPER_CONFIG_H_
