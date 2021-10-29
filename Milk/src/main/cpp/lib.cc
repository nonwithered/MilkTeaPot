#include <milk.h>

#include "launch.h"
#include "codec.h"
#include "dump.h"
#include "play.h"

namespace {

constexpr char TAG[] = "Milk#extern";

MilkTea_Logger_t MilkTea_CALL Milk_Inject_Logger_default(MilkTea_LogLevel_t level) {
  Milk::LoggerImpl &logger_ = Milk::LoggerImpl::Instance();
  logger_.level(MilkTea::Logger::LevelOf(level));
  return logger_.RawLogger();
}

MilkTea_Exception_t MilkTea_CALL Milk_Inject_SoyBean_Factory_default(SoyBean_Factory_t *) MilkTea_with_except({
  MilkTea_assert("Milk_Inject_SoyBean_Factory must be invoked before");
})

} // namespace

extern "C" {

MilkTea_API void MilkTea_CALL
Milk_Inject_Logger(MilkTea_Logger_t (MilkTea_CALL *inject)(MilkTea_LogLevel_t level)) {
  Milk::Inject::Logger(inject);
}

MilkTea_API void MilkTea_CALL
Milk_Inject_SoyBean_Factory(MilkTea_Exception_t (MilkTea_CALL *inject)(SoyBean_Factory_t *factory)) {
  Milk::Inject::SoyBean_Factory(inject);
}

MilkTea_API int MilkTea_CALL
Milk_Main(int argc, char *argv[]) try {
  std::list<std::string_view> args;
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  std::vector<std::unique_ptr<Milk::Command>> arr;
  arr.emplace_back(new Milk::Codec());
  arr.emplace_back(new Milk::Dump());
  arr.emplace_back(new Milk::Play());
  Milk::Command::LaunchMain(args, arr);
  return 0;
} catch (std::exception &e) {
  std::cerr << e.what() << std::endl;
  throw e;
}

} // extern "C"

namespace Milk {

LoggerImpl &LoggerImpl::Instance() {
  static Milk::LoggerImpl logger_;
  return logger_;
}

Inject::Logger_type Inject::Logger(Logger_type inject) {
  static Logger_type inject_ = Milk_Inject_Logger_default;
  if (inject != nullptr) {
    inject_ = inject;
  }
  return inject_;
}

Inject::SoyBean_Factory_type Inject::SoyBean_Factory(SoyBean_Factory_type inject) {
  static SoyBean_Factory_type inject_ = Milk_Inject_SoyBean_Factory_default;
  if (inject != nullptr) {
    inject_ = inject;
  }
  return inject_;
}

} // namespace Milk
