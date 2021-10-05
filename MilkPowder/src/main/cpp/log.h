#ifndef MILKPOWDER_LOG_H_
#define MILKPOWDER_LOG_H_

#define LOG_BUFFER_SIZE 128

#ifdef NDEBUG
#define LOG_PRINT(L, TAG, ...) \
do { \
} while (false)
#else // ifdef NDEBUG
#define LOG_PRINT(L, TAG, ...) \
do { \
  MilkPowder::Log log = MilkPowder::Log::Instance(); \
  if (log.level() <= MilkPowder::Log::Level::L) { \
    char buf[LOG_BUFFER_SIZE]; \
    snprintf(buf, LOG_BUFFER_SIZE, ##__VA_ARGS__); \
    log(MilkPowder::Log::Level::L, TAG, buf); \
  } \
} while (false)
#endif // ifdef NDEBUG

#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <functional>

namespace MilkPowder {

class Log final {
 public:
  enum class Level : uint8_t {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    ASSERT
  };
  static Log &Instance(Log *logger = nullptr);
  Log(
    std::function<void(const char *, const char *)> debug,
    std::function<void(const char *, const char *)> info,
    std::function<void(const char *, const char *)> warn,
    std::function<void(const char *, const char *)> error,
    Level level
  ) : debug_(debug), info_(info), warn_(warn), error_(error), level_(level) {}
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
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_LOG_H_
