#include <milk.h>

#include <soybean_windows.h>

namespace {

struct ConfigImpl {
  static Milk::LoggerImpl make_logger(MilkTea::Logger::Level level) {
    return Milk::LoggerImpl(level);
  }
  static SoyBean::FactoryWrapper make_soybean_factory() {
    return SoyBean_Windows::make_factory(0, nullptr, nullptr, 0);
  }
};

} // namespace

int main(int argc, char *argv[]) {
  Milk::Init<ConfigImpl>();
  return Milk::Main(argc, argv);
}
