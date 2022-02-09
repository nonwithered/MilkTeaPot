#include <milktea/common.h>

#include "except.h"

namespace {

constexpr char TAG[] = "MilkTea";

MilkTea::BaseLogger &BaseLogger_cast(void *obj) {
  MilkTea_nonnull(obj);
  return *reinterpret_cast<MilkTea::BaseLogger *>(obj);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Deleter(void *obj) {
  std::move(BaseLogger_cast(obj)).Destroy();
}

void MilkTea_call MilkTea_BaseLogger_Interface_Debug(void *obj, const char *tag, const char *msg) {
  BaseLogger_cast(obj).Debug(tag, msg);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Info(void *obj, const char *tag, const char *msg) {
  BaseLogger_cast(obj).Info(tag, msg);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Warn(void *obj, const char *tag, const char *msg) {
  BaseLogger_cast(obj).Warn(tag, msg);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Error(void *obj, const char *tag, const char *msg) {
  BaseLogger_cast(obj).Error(tag, msg);
}

} // namespace

namespace MilkTea {

const MilkTea_Logger_Interface_t &BaseLogger::Interface() {
  static const MilkTea_Logger_Interface_t instance_{
    .Deleter = MilkTea_BaseLogger_Interface_Deleter,
    .Debug = MilkTea_BaseLogger_Interface_Debug,
    .Info = MilkTea_BaseLogger_Interface_Info,
    .Warn = MilkTea_BaseLogger_Interface_Warn,
    .Error = MilkTea_BaseLogger_Interface_Error,
  };
  return instance_;
}

} // namespace MilkTea
