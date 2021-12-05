#include <milk.h>

#include "config.h"
#include "launch.h"
#include "codec.h"
#include "dump.h"
#include "play.h"

namespace {

constexpr char TAG[] = "Milk#extern";

std::list<std::string_view> Args(int argc, char *argv[]) {
  std::list<std::string_view> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  return args;
}

std::vector<std::unique_ptr<Milk::Command>> Cmds() {
  std::vector<std::unique_ptr<Milk::Command>> cmds;
  cmds.emplace_back(new Milk::Codec());
  cmds.emplace_back(new Milk::Dump());
  cmds.emplace_back(new Milk::Play());
  return cmds;
}

} // namespace

extern "C" {

MilkTea_extern(Milk_Init, (Milk_Config_t config), {
  Milk::ConfigWrapper::Instance(new Milk::ConfigWrapper(config));
})

MilkTea_api int MilkTea_call
Milk_Main(int argc, char *argv[]) try {
  Milk::ConfigWrapper::Instance();
  Milk::Command::LaunchMain(Args(argc, argv), Cmds());
  return 0;
} catch (std::exception &e) {
  std::cerr << e.what() << std::endl;
  throw e;
}

} // extern "C"

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
