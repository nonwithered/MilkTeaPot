#include <milktea.h>

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

class DefaultImpl final : public MilkTea::BaseLogger {
 public:
  static DefaultImpl &Instance();
  void Destroy() && final {
    delete this;
  }
  BaseLogger &Move() && final {
    return *new DefaultImpl(std::forward<DefaultImpl>(*this));
  }
  void Debug(std::string_view, std::string_view) final {}
  void Info(std::string_view, std::string_view) final {}
  void Warn(std::string_view, std::string_view) final {}
  void Error(std::string_view, std::string_view) final {}
 private:
  DefaultImpl() : MilkTea::BaseLogger(MilkTea::Logger::Level::ASSERT) {}
};

DefaultImpl &DefaultImpl::Instance() {
  static DefaultImpl instance_{};
  return instance_;
}

} // namespace

MilkTea::BaseLogger *GlobalInstance(MilkTea::LoggerWrapper *instance) {
  static auto instance_ = std::unique_ptr<MilkTea::BaseLogger>(nullptr);
  if (instance != nullptr) {
    if (instance_ != nullptr) {
      return nullptr;
    }
    instance_ = std::unique_ptr<MilkTea::BaseLogger>(instance);
  }
  if (instance_ == nullptr) {
    return &DefaultImpl::Instance();
  }
  return instance_.get();
}

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
