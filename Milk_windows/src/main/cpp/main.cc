#include <milk.h>

#include <milk_console.h>

#include "factory.h"

namespace Milk_Windows {

class FoundationImpl final : public Milk_Console::BaseFoundation {
 public:
  explicit FoundationImpl(std::string_view work_path) : work_path_(work_path) {}
  SoyBean_Factory_t GetSoyBeanFactory() {
    return FactoryImpl(work_path_).ToRawType();
  }
 private:
  const std::string_view work_path_;
  MilkTea_NonCopy(FoundationImpl)
  MilkTea_NonMove(FoundationImpl)
};

} // namespace Milk_Console

int main(int argc, char *argv[]) {
  if (argc == 0) {
    return EXIT_SUCCESS;
  }
  if (argc == 2 && std::string_view(argv[1]) == Milk_Windows::RemoteImpl::TAG) {
    Milk_Windows::RemoteImpl::Loop();
    return EXIT_SUCCESS;
  }
  Milk_Windows::FoundationImpl foundation(argv[0]);
  auto e = MilkTea::Exception::Suppress([&]() {
    Milk::Main(argc, argv, Milk_Console::make_context(foundation));
  });
  if (e == MilkTea::Exception::Type::Nil) {
    return EXIT_SUCCESS;
  }
  std::cerr << MilkTea::Exception::TypeName(e) << ": " << MilkTea::Exception::What() << std::endl;
  return EXIT_FAILURE;
}
