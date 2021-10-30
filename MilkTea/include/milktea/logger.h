#ifndef LIB_MILKTEA_LOGGER_H_
#define LIB_MILKTEA_LOGGER_H_

#include <milktea/util.h>

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_LogLevel_t {
  MilkTea_LogLevel_DEBUG,
  MilkTea_LogLevel_INFO,
  MilkTea_LogLevel_WARN,
  MilkTea_LogLevel_ERROR,
  MilkTea_LogLevel_ASSERT
};

#ifndef __cplusplus
typedef enum MilkTea_LogLevel_t MilkTea_LogLevel_t;
#endif // ifndef __cplusplus

struct MilkTea_Logger_t {
  void *obj;
  enum MilkTea_LogLevel_t level;
  void (MilkTea_CALL *debug)(void *obj, const char *tag, const char *msg);
  void (MilkTea_CALL *info)(void *obj, const char *tag, const char *msg);
  void (MilkTea_CALL *warn)(void *obj, const char *tag, const char *msg);
  void (MilkTea_CALL *error)(void *obj, const char *tag, const char *msg);
};

#ifndef __cplusplus
typedef struct MilkTea_Logger_t MilkTea_Logger_t;
#endif // ifndef __cplusplus

MilkTea_API void MilkTea_CALL
MilkTea_Logger_Init(MilkTea_Logger_t log);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <functional>
#include <cstdio>
namespace MilkTea {

#ifdef NDEBUG
#define MilkTea_LogPrint(L, TAG, ...) MilkTea_block({})
#else // ifdef NDEBUG
#define MilkTea_LogPrint(LEVEL, TAG, ...) MilkTea_block({ \
  MilkTea::Logger &logger = MilkTea::Logger::Instance(); \
  MilkTea::Logger::Level level = MilkTea::Logger::Level::LEVEL; \
  if (!logger.Available(level)) { \
    break; \
  } \
  const size_t len = snprintf(nullptr, 0, ##__VA_ARGS__) + 1; \
  char msg[len]; \
  snprintf(msg, len, ##__VA_ARGS__); \
  logger.Print(level, TAG, msg); \
})
#endif // ifdef NDEBUG

#define MilkTea_logD(...) \
MilkTea_LogPrint(DEBUG, TAG, ##__VA_ARGS__)

#define MilkTea_logI(...) \
MilkTea_LogPrint(INFO, TAG, ##__VA_ARGS__)

#define MilkTea_logW(...) \
MilkTea_LogPrint(WARN, TAG, ##__VA_ARGS__)

#define MilkTea_logE(...) \
MilkTea_LogPrint(ERROR, TAG, ##__VA_ARGS__)

class Logger final {
 public:
  enum class Level : uint8_t {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    ASSERT
  };
  static Level LevelOf(MilkTea_LogLevel_t level) {
    switch (level) {
      case MilkTea_LogLevel_t::MilkTea_LogLevel_DEBUG: return Level::DEBUG;
      case MilkTea_LogLevel_t::MilkTea_LogLevel_INFO: return Level::INFO;
      case MilkTea_LogLevel_t::MilkTea_LogLevel_WARN: return Level::WARN;
      case MilkTea_LogLevel_t::MilkTea_LogLevel_ERROR: return Level::ERROR;
      default: return Level::ASSERT;
    }
  }
  MilkTea_API static Logger & MilkTea_CALL Instance(Logger *logger = nullptr);
  Logger(
    Level level,
    std::function<void(const char *, const char *)> debug,
    std::function<void(const char *, const char *)> info,
    std::function<void(const char *, const char *)> warn,
    std::function<void(const char *, const char *)> error
  ) : level_(level), debug_(debug), info_(info), warn_(warn), error_(error) {}
  void Print(Level level, const char *tag, const char *msg) {
    switch (level) {
      case Level::DEBUG: debug_(tag, msg); break;
      case Level::INFO: info_(tag, msg); break;
      case Level::WARN: warn_(tag, msg); break;
      case Level::ERROR: error_(tag, msg); break;
      default: break;
    }
  }
  bool Available(Level level) const {
    return level_ <= level;
  }
 private:
  Level level_;
  std::function<void(const char *, const char *)> debug_;
  std::function<void(const char *, const char *)> info_;
  std::function<void(const char *, const char *)> warn_;
  std::function<void(const char *, const char *)> error_;
};

class BaseLogger {
 public:
  static MilkTea_LogLevel_t RawLevel(Logger::Level level) {
    switch (level) {
      case Logger::Level::DEBUG: return MilkTea_LogLevel_t::MilkTea_LogLevel_DEBUG;
      case Logger::Level::INFO: return MilkTea_LogLevel_t::MilkTea_LogLevel_INFO;
      case Logger::Level::WARN: return MilkTea_LogLevel_t::MilkTea_LogLevel_WARN;
      case Logger::Level::ERROR: return MilkTea_LogLevel_t::MilkTea_LogLevel_ERROR;
      default: return MilkTea_LogLevel_t::MilkTea_LogLevel_ASSERT;
    }
  }
  MilkTea_API MilkTea_Logger_t MilkTea_CALL RawLogger();
  virtual Logger::Level level() const = 0;
  virtual void Debug(const char *tag, const char *msg) = 0;
  virtual void Info(const char *tag, const char *msg) = 0;
  virtual void Warn(const char *tag, const char *msg) = 0;
  virtual void Error(const char *tag, const char *msg) = 0;
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_LOGGER_H_
