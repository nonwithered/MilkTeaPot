#ifndef MILKTEA_LOGGER_H_
#define MILKTEA_LOGGER_H_

#include <milktea/common.h>

namespace MilkTea {

class LoggerWrapper final : public BaseLogger {
 public:
  MilkTea_Logger_t ToRawType() && final {
    return release();
  }
  LoggerWrapper(MilkTea_Logger_t another) : BaseLogger(Logger::FromRawType(another.level_)), obj_(another) {}
  LoggerWrapper(LoggerWrapper &&another) : LoggerWrapper(another.level()) {
    std::swap(obj_, another.obj_);
    another.~LoggerWrapper();
  }
  ~LoggerWrapper() final {
    if (obj() == nullptr) {
      return;
    }
    Interface().Deleter(obj());
    obj_ = {};
  }
  BaseLogger &Move() && final {
    return *new LoggerWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void Debug(std::string_view tag, std::string_view msg) final {
    Interface().Debug(obj(), tag.data(), msg.data());
  }
  void Info(std::string_view tag, std::string_view msg) final {
    Interface().Info(obj(), tag.data(), msg.data());
  }
  void Warn(std::string_view tag, std::string_view msg) final {
    Interface().Warn(obj(), tag.data(), msg.data());
  }
  void Error(std::string_view tag, std::string_view msg) final {
    Interface().Error(obj(), tag.data(), msg.data());
  }
  MilkTea_Logger_t release() {
    MilkTea_Logger_t obj = {};
    std::swap(obj, obj_);
    return obj;
  }
 private:
  void *obj() const {
    return obj_.obj_;
  }
  const MilkTea_Logger_Interface_t &Interface() const {
    return *obj_.interface_;
  }
  LoggerWrapper() : LoggerWrapper(Logger::Level::ASSERT) {}
  LoggerWrapper(Logger::Level level) : BaseLogger(level), obj_{nullptr, Logger::ToRawType(Logger::Level::ASSERT), nullptr} {}
  MilkTea_Logger_t obj_;
  MilkTea_NonCopy(LoggerWrapper)
  MilkTea_NonMoveAssign(LoggerWrapper)
};

} // namespace MilkTea

#endif // ifndef MILKTEA_LOGGER_H_
