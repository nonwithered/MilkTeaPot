#ifndef MILKPOWDER_LOG_H_
#define MILKPOWDER_LOG_H_

#include <cstdint>
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
  static Log Instance(Log *logger = nullptr);
  static void Print(Level level, const char *tag, const char *msg) {
    Instance()(level, tag, msg);
  }
  Log(
    std::function<void(const char *, const char *)> debug,
    std::function<void(const char *, const char *)> info,
    std::function<void(const char *, const char *)> warn,
    std::function<void(const char *, const char *)> error,
    Level level
  ) : debug_(debug), info_(info), warn_(warn), error_(error), level_(level) {}
 private:
  void operator()(Level level, const char *tag, const char *msg) {
    if (level < level_) {
      return;
    }
    switch (level) {
      case Level::DEBUG: debug_(tag, msg); break;
      case Level::INFO: info_(tag, msg); break;
      case Level::WARN: warn_(tag, msg); break;
      case Level::ERROR: error_(tag, msg); break;
      default: break;
    }
  }
  std::function<void(const char *, const char *)> debug_;
  std::function<void(const char *, const char *)> info_;
  std::function<void(const char *, const char *)> warn_;
  std::function<void(const char *, const char *)> error_;
  Level level_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_LOG_H_
