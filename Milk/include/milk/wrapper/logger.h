#ifndef LIB_MILK_LOGGER_H_
#define LIB_MILK_LOGGER_H_

#include <iostream>
#include <string>
#include <string_view>
#include <ctime>
#include <sstream>

#include <milktea.h>

namespace Milk {

class LoggerImpl : public MilkTea::BaseLogger {
 public:
  LoggerImpl(MilkTea::Logger::Level level) : MilkTea::BaseLogger(level) {}
  BaseLogger &Move() && final {
    return *new LoggerImpl(std::forward<LoggerImpl>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void Debug(std::string_view tag, std::string_view msg) final {
    std::cerr << Current() << "/DEBUG: " << tag << ": " << msg << std::endl;
  }
  void Info(std::string_view tag, std::string_view msg) final {
    std::cerr << Current() << "/INFO: " << tag << ": " << msg << std::endl;
  }
  void Warn(std::string_view tag, std::string_view msg) final {
    std::cerr << Current() << "/WARN: " << tag << ": " << msg << std::endl;
  }
  void Error(std::string_view tag, std::string_view msg) final {
    std::cerr << Current() << "/ERROR: " << tag << ": " << msg << std::endl;
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

} // namespace Milk

#endif // ifndef LIB_MILK_LOGGER_H_
