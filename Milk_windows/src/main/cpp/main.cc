#include <milk.h>

#include <milk_console.h>
#include <soybean_windows.h>

namespace Milk_Windows {

class FoundationImpl final : public Milk_Console::BaseFoundation {
  SoyBean::FactoryWrapper GetSoyBeanFactory() {
    return SoyBean_Windows::make_factory(0, nullptr, nullptr, 0);
  }
};

} // namespace Milk_Console

int main(int argc, char *argv[]) {
  Milk_Windows::FoundationImpl foundation;
  Milk_Console::ContextHolder context(foundation);
  Milk::Main(argc, argv, context);
  return 0;
}
