#include <milk.h>

#include <milk_console.h>
#include <soybean_windows.h>

namespace Milk_Windows {

class FoundationImpl final : public Milk_Console::BaseFoundation {
  SoyBean_Factory_t GetSoyBeanFactory() {
    return SoyBean_Windows::make_factory(0, nullptr, nullptr, 0).ToRawType();
  }
};

} // namespace Milk_Console

int main(int argc, char *argv[]) {
  Milk_Windows::FoundationImpl foundation;
  auto e = MilkTea::Exception::Catch([&]() {
    Milk::Main(argc, argv, Milk_Console::ContextHolder(foundation).release());
  });
  if (e == MilkTea::Exception::Type::Nil) {
    return EXIT_SUCCESS;
  }
  std::cerr << MilkTea::Exception::TypeName(e) << ": " << MilkTea::Exception::What() << std::endl;
  return EXIT_FAILURE;
}
