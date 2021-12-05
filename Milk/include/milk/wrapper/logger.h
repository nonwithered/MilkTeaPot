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
    time_t t = time(nullptr);
    tm *p = localtime(&t);
    std::stringstream ss[2];
    std::string s[2];
    ss[0] << (p->tm_year + 1900) << "-" << (p->tm_mon + 1) << "-" << p->tm_mday;
    ss[0] >> s[0];
    ss[1] << p->tm_hour << ":" << p->tm_min << ":" << p->tm_sec;
    ss[1] >> s[1];
    return s[0] + " " + s[1];
  }
};

} // namespace Milk

#endif // ifndef LIB_MILK_LOGGER_H_
