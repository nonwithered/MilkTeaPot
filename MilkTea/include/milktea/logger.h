#ifndef LIB_MILKTEA_LOGGER_H_
#define LIB_MILKTEA_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_LogLevel_t {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT
};

struct MilkTea_Logger_t {
  void *obj;
  void (*debug)(void *obj, const char *tag, const char *msg);
  void (*info)(void *obj, const char *tag, const char *msg);
  void (*warn)(void *obj, const char *tag, const char *msg);
  void (*error)(void *obj, const char *tag, const char *msg);
  enum MilkTea_LogLevel_t level;
};

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
typedef enum MilkTea_LogLevel_t MilkTea_LogLevel_t;
typedef struct MilkTea_Logger_t MilkTea_Logger_t;
#endif // ifndef __cplusplus

#ifdef __cplusplus
#include <functional>
#include <cstdio>
namespace MilkTea {

constexpr size_t kLogMaxSize = 128;

#define MilkTea_Logger_Log_Decl(P) \
namespace P { \
MilkTea::Logger &Log(MilkTea::Logger *logger = nullptr); \
} /* namespace */

#define MilkTea_Logger_Log_Impl(P) \
MilkTea::Logger &P::Log(MilkTea::Logger *logger) { \
  static MilkTea::Logger logger_; \
  if (logger != nullptr) { \
    logger_ = *logger; \
  } \
  return logger_; \
}

#ifdef NDEBUG
#define MilkTea_Logger_LogPrint(P, L, TAG, ...) \
do { \
} while (false)
#else // ifdef NDEBUG
#define MilkTea_Logger_LogPrint(P, L, TAG, ...) \
do { \
  MilkTea::Logger logger = P::Log(); \
  if (logger.level() <= MilkTea::Logger::Level::L) { \
    char msg[MilkTea::kLogMaxSize]; \
    snprintf(msg, MilkTea::kLogMaxSize, ##__VA_ARGS__); \
    logger(MilkTea::Logger::Level::L, TAG, msg); \
  } \
} while (false)
#endif // ifdef NDEBUG

class Logger final {
 public:
  enum class Level : uint8_t {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    ASSERT
  };
  Logger(
    std::function<void(const char *, const char *)> debug,
    std::function<void(const char *, const char *)> info,
    std::function<void(const char *, const char *)> warn,
    std::function<void(const char *, const char *)> error,
    Level level
  ) : debug_(debug), info_(info), warn_(warn), error_(error), level_(level) {}
  Logger(MilkTea_Logger_t logger) : Logger(
    LogCallback(logger.obj, logger.debug),
    LogCallback(logger.obj, logger.info),
    LogCallback(logger.obj, logger.warn),
    LogCallback(logger.obj, logger.error),
    LevelOf(logger.level)
  ) {}
  Logger() : Logger(
    LogDefault, 
    LogDefault, 
    LogDefault, 
    LogDefault, 
    Level::ASSERT
  ) {}
  Level level() const {
    return level_;
  }
  void operator()(Level level, const char *tag, const char *msg) {
    switch (level) {
      case Level::DEBUG: debug_(tag, msg); break;
      case Level::INFO: info_(tag, msg); break;
      case Level::WARN: warn_(tag, msg); break;
      case Level::ERROR: error_(tag, msg); break;
      default: break;
    }
  }
 private:
  std::function<void(const char *, const char *)> debug_;
  std::function<void(const char *, const char *)> info_;
  std::function<void(const char *, const char *)> warn_;
  std::function<void(const char *, const char *)> error_;
  Level level_;
  static std::function<void(const char *, const char *)> LogCallback(void *obj, void (*callback)(void *obj, const char *tag, const char *msg)) {
    return [obj, callback](const char *tag, const char *msg) -> void {
      callback(obj, tag, msg);
    };
  }
  static Level LevelOf(MilkTea_LogLevel_t level) {
    switch (level) {
        case MilkTea_LogLevel_t::DEBUG: return Level::DEBUG;
        case MilkTea_LogLevel_t::INFO: return Level::INFO;
        case MilkTea_LogLevel_t::WARN: return Level::WARN;
        case MilkTea_LogLevel_t::ERROR: return Level::ERROR;
        default: return Level::ASSERT;
    }
  }
  static void LogDefault(const char *, const char *) {}
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_LOGGER_H_
