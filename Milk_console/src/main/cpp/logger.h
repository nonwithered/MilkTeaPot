#ifndef MILK_CONSOLE_LOGGER_H_
#define MILK_CONSOLE_LOGGER_H_

#include <milk_console/common.h>

namespace Milk_Console {

class LoggerImpl : public MilkTea::BaseLogger {
 public:
  LoggerImpl(MilkTea::Logger::Level level) : MilkTea::BaseLogger(level) {}
  BaseLogger &Move() && final {
    return *new LoggerImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void Debug(std::string_view tag, std::string_view msg) final {
    std::clog << Current() << "/DEBUG: " << tag << ": " << msg << std::endl;
  }
  void Info(std::string_view tag, std::string_view msg) final {
    std::clog << Current() << "/INFO: " << tag << ": " << msg << std::endl;
  }
  void Warn(std::string_view tag, std::string_view msg) final {
    std::clog << Current() << "/WARN: " << tag << ": " << msg << std::endl;
  }
  void Error(std::string_view tag, std::string_view msg) final {
    std::clog << Current() << "/ERROR: " << tag << ": " << msg << std::endl;
  }
 private:
  static std::string Current() {
    auto t = std::time(nullptr);
    auto *p = std::localtime(&t);
    constexpr std::size_t kSize = 32;
    std::array<char, kSize> str{};
    std::strftime(str.data(), kSize, "%Y-%m-%d %H:%M:%S", p);
    return str.data();
  }
};

} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_LOGGER_H_
