#ifndef MILKTEA_LOGGER_H_
#define MILKTEA_LOGGER_H_

#include <milktea/common.h>

namespace MilkTea {

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

} // namespace MilkTea

#endif // ifndef MILKTEA_LOGGER_H_
