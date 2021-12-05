#ifndef LIB_MILKTEA_WRAPPER_LOGGER_H_
#define LIB_MILKTEA_WRAPPER_LOGGER_H_

#include <milktea/common.h>

namespace MilkTea {

class BaseLogger {
 public:
  virtual MilkTea_Logger_t ToRawType() && {
    auto level_ = Logger::ToRawType(level());
    return MilkTea_Logger_t{
      .self_ = &std::forward<BaseLogger>(*this).Move(),
      .level_ = level_,
      .interface_ = &Interface(),
    };
  }
  BaseLogger(Logger::Level level) : level_(level) {}
  virtual ~BaseLogger() {
    level_ = Logger::Level::ASSERT;
  }
  virtual BaseLogger &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void Debug(std::string_view tag, std::string_view msg) = 0;
  virtual void Info(std::string_view tag, std::string_view msg) = 0;
  virtual void Warn(std::string_view tag, std::string_view msg) = 0;
  virtual void Error(std::string_view tag, std::string_view msg) = 0;
  void Print(Logger::Level level, std::string_view tag, std::string_view msg) {
    switch (level) {
      case Logger::Level::DEBUG: Debug(tag, msg); return;
      case Logger::Level::INFO: Info(tag, msg); return;
      case Logger::Level::WARN: Warn(tag, msg); return;
      case Logger::Level::ERROR: Error(tag, msg); return;
      default: return;
    }
  }
  Logger::Level level() const {
    return level_;
  }
 private:
  static MilkTea_decl(const MilkTea_Logger_Interface_t &) Interface();
  Logger::Level level_;
};

class LoggerWrapper final : public BaseLogger {
 public:
  MilkTea_Logger_t ToRawType() && final {
    return release();
  }
  LoggerWrapper(MilkTea_Logger_t another) : BaseLogger(Logger::FromRawType(another.level_)), self_(another) {}
  LoggerWrapper(LoggerWrapper &&another) : LoggerWrapper(another.level()) {
    std::swap(self_, another.self_);
    another.~LoggerWrapper();
  }
  ~LoggerWrapper() final {
    if (Self() == nullptr) {
      return;
    }
    Interface().Deleter(Self());
    self_ = {};
  }
  BaseLogger &Move() && final {
    return *new LoggerWrapper(std::forward<LoggerWrapper>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void Debug(std::string_view tag, std::string_view msg) final {
    Interface().Debug(Self(), tag.data(), msg.data());
  }
  void Info(std::string_view tag, std::string_view msg) final {
    Interface().Info(Self(), tag.data(), msg.data());
  }
  void Warn(std::string_view tag, std::string_view msg) final {
    Interface().Warn(Self(), tag.data(), msg.data());
  }
  void Error(std::string_view tag, std::string_view msg) final {
    Interface().Error(Self(), tag.data(), msg.data());
  }
  MilkTea_Logger_t release() {
    MilkTea_Logger_t self = self_;
    self_ = {};
    return self;
  }
 private:
  void *Self() const {
    return self_.self_;
  }
  const MilkTea_Logger_Interface_t &Interface() const {
    return *self_.interface_;
  }
  LoggerWrapper() : LoggerWrapper(Logger::Level::ASSERT) {}
  LoggerWrapper(Logger::Level level) : BaseLogger(level), self_{nullptr, Logger::ToRawType(Logger::Level::ASSERT), nullptr} {}
  MilkTea_Logger_t self_;
  MilkTea_NonCopy(LoggerWrapper)
  MilkTea_NonMoveAssign(LoggerWrapper)
};

namespace Logger {

template<typename T>
bool Config(T &&logger) {
  return MilkTea_Logger_Config(std::forward<T>(logger).ToRawType());
}

inline
Level GetLevel() {
  return FromRawType(MilkTea_Logger_GetLevel());
}

inline
void Print(Level level, std::string_view tag, std::string_view msg) {
  MilkTea_Logger_Print(ToRawType(level), tag.data(), msg.data());
}

} // namespace Logger

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_WRAPPER_LOGGER_H_
