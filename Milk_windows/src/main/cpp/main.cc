#include <milk.h>

#include <soybean_windows.h>

namespace {

struct ConfigImpl {
  static Milk::LoggerImpl MakeMilkTeaLogger(MilkTea::Logger::Level level) {
    return Milk::LoggerImpl(level);
  }
  static SoyBean::FactoryWrapper MakeSoyBeanFactory() {
    return SoyBean_Windows::CreateFactory(0, nullptr, nullptr, 0);
  }
};

} // namespace

int main(int argc, char *argv[]) {
  Milk::Init<ConfigImpl>();
  return Milk::Main(argc, argv);
}
