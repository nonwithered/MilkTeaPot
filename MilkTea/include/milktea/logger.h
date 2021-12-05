#ifndef LIB_MILKTEA_LOGGER_H_
#define LIB_MILKTEA_LOGGER_H_

#include <milktea/util.h>

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_Logger_Level_t {
  MilkTea_Logger_Level_DEBUG,
  MilkTea_Logger_Level_INFO,
  MilkTea_Logger_Level_WARN,
  MilkTea_Logger_Level_ERROR,
  MilkTea_Logger_Level_ASSERT
};

#ifndef __cplusplus
typedef enum MilkTea_Logger_Level_t MilkTea_Logger_Level_t;
#endif // ifndef __cplusplus

struct MilkTea_Logger_Interface_t {
  void (MilkTea_call *Deleter)(void *self);
  void (MilkTea_call *Debug)(void *self, const char *tag, const char *msg);
  void (MilkTea_call *Info)(void *self, const char *tag, const char *msg);
  void (MilkTea_call *Warn)(void *self, const char *tag, const char *msg);
  void (MilkTea_call *Error)(void *self, const char *tag, const char *msg);
};

#ifndef __cplusplus
typedef struct MilkTea_Logger_Interface_t MilkTea_Logger_Interface_t;
#endif // ifndef __cplusplus

struct MilkTea_Logger_t {
  void *self_;
  enum MilkTea_Logger_Level_t level_;
  const MilkTea_Logger_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct MilkTea_Logger_t MilkTea_Logger_t;
#endif // ifndef __cplusplus

MilkTea_api bool MilkTea_call
MilkTea_Logger_Config(MilkTea_Logger_t logger);

MilkTea_api MilkTea_Logger_Level_t MilkTea_call
MilkTea_Logger_GetLevel();

MilkTea_api void MilkTea_call
MilkTea_Logger_Print(MilkTea_Logger_Level_t level, const char *tag, const char *msg);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include <cstdio>
#include <cstddef>
#else // ifdef __cplusplus
#include <stdio.h>
#include <stddef.h>
#endif // ifdef __cplusplus

#ifdef NDEBUG
#define MilkTea_log(...) MilkTea_block({})
#else // ifdef NDEBUG
#define MilkTea_log(LEVEL, TAG, ...) MilkTea_block({ \
  MilkTea_Logger_Level_t level_ = MilkTea_Logger_GetLevel(); \
  if (level_ > MilkTea_Logger_Level_##LEVEL) { \
    break; \
  } \
  const size_t size_ = snprintf(NULL, 0, ##__VA_ARGS__) + 1; \
  char msg_[size_]; \
  snprintf(msg_, size_, ##__VA_ARGS__); \
  MilkTea_Logger_Print(MilkTea_Logger_Level_##LEVEL, TAG, msg_); \
})
#endif // ifdef NDEBUG

#define MilkTea_logD(...) \
MilkTea_log(DEBUG, TAG, ##__VA_ARGS__)

#define MilkTea_logI(...) \
MilkTea_log(INFO, TAG, ##__VA_ARGS__)

#define MilkTea_logW(...) \
MilkTea_log(WARN, TAG, ##__VA_ARGS__)

#define MilkTea_logE(...) \
MilkTea_log(ERROR, TAG, ##__VA_ARGS__)

#ifdef __cplusplus
#include <functional>
#include <memory>
namespace MilkTea {

namespace Logger {

enum class Level {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT
};

static MilkTea_Logger_Level_t ToRawType(Level level) {
  switch (level) {
  case Level::DEBUG: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG;
  case Level::INFO: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO;
  case Level::WARN: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN;
  case Level::ERROR: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR;
  default: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_ASSERT;
  }
}

static Level FromRawType(MilkTea_Logger_Level_t level) {
  switch (level) {
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG: return Level::DEBUG;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO: return Level::INFO;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN: return Level::WARN;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR: return Level::ERROR;
  default: return Level::ASSERT;
  }
}

}; // namespace Logger

class BaseLogger {
 public:
  MilkTea_Logger_t ToRawType() && {
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
  static MilkTea_api const MilkTea_Logger_Interface_t & MilkTea_call Interface();
  Logger::Level level_;
};

class LoggerWrapper final : public BaseLogger {
 public:
  MilkTea_Logger_t ToRawType() && {
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
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_LOGGER_H_
