#include <milk.h>

#include "launch.h"

namespace {

constexpr char TAG[] = "Milk";

} // namespace

MilkTea_extern(Milk_Init, (Milk_Config_t config), {
  Milk::ConfigWrapper::Instance(new Milk::ConfigWrapper(config));
})

MilkTea_decl(int)
Milk_Main(int argc, char *argv[]) {
  return Milk::Launcher::Launch(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}

namespace Milk {

ConfigWrapper &ConfigWrapper::Instance(ConfigWrapper *instance) {
  static std::unique_ptr<ConfigWrapper> instance_;
  if (instance != nullptr) {
    if (instance_ != nullptr) {
      delete instance;
      MilkTea_throw(LogicError, "Config Init again");
    }
    instance_ = std::unique_ptr<ConfigWrapper>(instance);
  }
  if (instance_ == nullptr) {
    MilkTea_throw(LogicError, "Config need Init");
  }
  return *instance_;
}

} // namespace Milk
