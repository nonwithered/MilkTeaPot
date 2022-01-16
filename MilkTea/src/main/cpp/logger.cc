#include <milktea/common.h>

#include "except.h"

namespace {

constexpr char TAG[] = "MilkTea";

MilkTea::BaseLogger &BaseLogger_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<MilkTea::BaseLogger *>(self);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Deleter(void *self) {
  std::move(BaseLogger_cast(self)).Destroy();
}

void MilkTea_call MilkTea_BaseLogger_Interface_Debug(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Debug(tag, msg);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Info(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Info(tag, msg);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Warn(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Warn(tag, msg);
}

void MilkTea_call MilkTea_BaseLogger_Interface_Error(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Error(tag, msg);
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
