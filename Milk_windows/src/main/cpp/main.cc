#include <milk.h>
#include <milk_console.h>

#include <tea.h>

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

struct my_logger : tea::Drop<my_logger>, tea::remove_copy {
  my_logger() {
    std::cerr << "my_logger ctor" << std::endl;
  }
  ~my_logger() {
    std::cerr << "my_logger dtor" << std::endl;
  }
  auto drop() && -> void {
    std::cerr << "my_logger drop" << std::endl;
  }
  auto Print(tea::log_level level, const char tag[], const char msg[]) {
    std::cerr << "my_logger Print" << std::endl;
    fprintf(stderr, "%d %s %s\n", level, tag, msg);
  }
  auto Priority() const -> tea::log_level {
    std::cerr << "my_logger Print" << std::endl;
    return tea::log_level::I;
  }
};

template<>
struct tea::meta::cast_pair<my_logger> {
  using type = tea::log_ctx;
};

struct my_log_test : tea::with_log<my_log_test> {
  static
  auto TAG() -> const char * {
    return "my_log_test";
  }
  my_log_test() {
    log<D>("D");
    log<I>("I");
    log<W>("W");
    log<E>("E");
  }
};

int main(int argc, char *argv[]) {
//  if (argc == 0) {
//    return EXIT_SUCCESS;
//  }
//  if (argc == 2 && std::string_view(argv[1]) == Milk_Windows::RemoteImpl::TAG) {
//    Milk_Windows::RemoteImpl::Loop();
//    return EXIT_SUCCESS;
//  }
//  Milk_Windows::FoundationImpl foundation(argv[0]);
//  auto e = MilkTea::Exception::Suppress([&]() {
//    Milk::Main(argc, argv, Milk_Console::make_context(foundation));
//  });
//  if (e == MilkTea::Exception::Type::Nil) {
//    return EXIT_SUCCESS;
//  }
//  std::cerr << MilkTea::Exception::TypeName(e) << ": " << MilkTea::Exception::What() << std::endl;
//  return EXIT_FAILURE;
  auto f = []() {
    auto e = tea::err::init();
    if (e != nullptr) {
      std::cout << e->str() << std::endl;
      tea::drop(e);
    } else {
      std::cout << "success" << std::endl;
    }
  };
  tea::logger::setup(*new my_logger);
  f();
  my_log_test a;
  tea_logger_t n{};
  tea_logger_setup(&n);
  f();
  std::cout << "main" << std::endl;
}
