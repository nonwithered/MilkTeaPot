#include <milk.h>

#include "config.h"
#include "launch.h"
#include "codec.h"
#include "dump.h"
#include "play.h"

namespace {

constexpr char TAG[] = "Milk";

} // namespace

MilkTea_extern(Milk_Init, (Milk_Config_t config), {
  Milk::ConfigWrapper::Instance(new Milk::ConfigWrapper(config));
})

MilkTea_decl(int)
Milk_Main(int argc, char *argv[]) {
  Milk::ConfigWrapper::Instance();
  Milk::Command::LaunchMain([argc, argv]() -> auto {
    std::list<std::string_view> args;
    for (int i = 1; i < argc; ++i) {
      args.emplace_back(argv[i]);
    }
    return args;
  }(), {
    std::make_unique<Milk::Codec>(),
    std::make_unique<Milk::Dump>(),
    std::make_unique<Milk::Play>(),
  });
  return 0;
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
