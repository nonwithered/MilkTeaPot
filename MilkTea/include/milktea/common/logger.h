#ifndef LIB_MILKTEA_COMMON_LOGGER_H_
#define LIB_MILKTEA_COMMON_LOGGER_H_

#include <functional>
#include <memory>

#include <milktea/core.h>

namespace MilkTea {

namespace Logger {

enum class Level {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT
};

inline MilkTea_Logger_Level_t ToRawType(Level level) {
  switch (level) {
  case Level::DEBUG: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG;
  case Level::INFO: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO;
  case Level::WARN: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN;
  case Level::ERROR: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR;
  default: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_ASSERT;
  }
}

inline Level FromRawType(MilkTea_Logger_Level_t level) {
  switch (level) {
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG: return Level::DEBUG;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO: return Level::INFO;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN: return Level::WARN;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR: return Level::ERROR;
  default: return Level::ASSERT;
  }
}

} // namespace Logger

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

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_LOGGER_H_
