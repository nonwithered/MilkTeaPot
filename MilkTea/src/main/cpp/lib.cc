#include <milktea.h>

namespace {

constexpr char TAG[] = "MilkTea#extern";

MilkTea::BaseLogger &BaseLogger_cast(void *self) {
  MilkTea_nonnull(self);
  return *reinterpret_cast<MilkTea::BaseLogger *>(self);
}

void MilkTea_CALL MilkTea_BaseLogger_Interface_Deleter(void *self) {
  std::move(BaseLogger_cast(self)).Destroy();
}

void MilkTea_CALL MilkTea_BaseLogger_Interface_Debug(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Debug(tag, msg);
}

void MilkTea_CALL MilkTea_BaseLogger_Interface_Info(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Info(tag, msg);
}

void MilkTea_CALL MilkTea_BaseLogger_Interface_Warn(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Warn(tag, msg);
}

void MilkTea_CALL MilkTea_BaseLogger_Interface_Error(void *self, const char *tag, const char *msg) {
  BaseLogger_cast(self).Error(tag, msg);
}

} // namespace

namespace MilkTea {

namespace Logger {

class DefaultImpl final : public BaseLogger {
 public:
  static DefaultImpl &Instance();
  void Destroy() && final {
    delete this;
  }
  std::unique_ptr<BaseLogger> Move() && final {
    return std::make_unique<DefaultImpl>(std::forward<DefaultImpl>(*this));
  }
  void Debug(std::string_view, std::string_view) final {}
  void Info(std::string_view, std::string_view) final {}
  void Warn(std::string_view, std::string_view) final {}
  void Error(std::string_view, std::string_view) final {}
 private:
  DefaultImpl() : BaseLogger(Level::ASSERT) {}
};

DefaultImpl &DefaultImpl::Instance() {
  static DefaultImpl instance_{};
  return instance_;
}

BaseLogger *Instance(LoggerWrapper *instance = nullptr) {
  static auto instance_ = std::unique_ptr<BaseLogger>(nullptr);
  if (instance != nullptr) {
    if (instance_ != nullptr) {
      return nullptr;
    }
    instance_ = std::unique_ptr<BaseLogger>(instance);
  }
  if (instance_ == nullptr) {
    return &DefaultImpl::Instance();
  }
  return instance_.get();
}

} // namespace Logger

const char *Exception::WhatMessage(const char *what) {
  thread_local static std::string what_;
  if (what != nullptr) {
    what_ = what;
  }
  return what_.data();
}

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

extern "C" {

const char *MilkTea_Exception_What() {
  return MilkTea::Exception::WhatMessage();
}

bool MilkTea_Logger_Config(MilkTea_Logger_t logger) {
  auto *logger_ = new MilkTea::LoggerWrapper(MilkTea::LoggerWrapper::FromRawType(std::move(logger)));
  if (MilkTea::Logger::Instance(logger_) == nullptr) {
    delete logger_;
    return false;
  }
  return true;
}

MilkTea_Logger_Level_t MilkTea_Logger_GetLevel() {
  return MilkTea::Logger::ToRawType(MilkTea::Logger::Instance()->level());
}

void MilkTea_Logger_Print(MilkTea_Logger_Level_t level, const char *tag, const char *msg) {
  MilkTea::Logger::Instance()->Print(MilkTea::Logger::FromRawType(level), tag, msg);
}

} // extern "C"
