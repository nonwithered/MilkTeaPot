#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iostream>

#include <milktea.h>
#include <soybean.h>

#define check_err(s) MilkTea_block({ \
  if (err != MilkTea_Exception_t::MilkTea_Exception_Nil) { \
    std::cerr << "Failed to " s " because of " << MilkTea_Exception_Name(err) << ": " << MilkTea_Exception_What() << std::endl; \
    exit(0); \
  } \
})

namespace Milk {

constexpr char kVersion[] = "0.0.1-snapshot";

inline std::string LogTime() {
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

class LoggerImpl : public MilkTea::BaseLogger {
 public:
  static LoggerImpl &Instance();
  void level(MilkTea::Logger::Level level) {
    level_ = level;
  }
  LoggerImpl(): level_(MilkTea::Logger::Level::ASSERT) {}
  MilkTea::Logger::Level level() const final {
    return level_;
  }
  void Debug(const char *tag, const char *msg) final {
    std::cerr << LogTime() << "/DEBUG: " << tag << ": " << msg << std::endl;
  }
  void Info(const char *tag, const char *msg) final {
    std::cerr << LogTime() << "/INFO: " << tag << ": " << msg << std::endl;
  }
  void Warn(const char *tag, const char *msg) final {
    std::cerr << LogTime() << "/WARN: " << tag << ": " << msg << std::endl;
  }
  void Error(const char *tag, const char *msg) final {
    std::cerr << LogTime() << "/ERROR: " << tag << ": " << msg << std::endl;
  }
 private:
  MilkTea::Logger::Level level_;
};

struct Inject {
  using Logger_type = MilkTea_Logger_t (MilkTea_CALL *)(MilkTea_LogLevel_t level);
  static Logger_type Logger(Logger_type inject = nullptr);
  using SoyBean_Factory_type = MilkTea_Exception_t (MilkTea_CALL *)(SoyBean_Factory_t *factory);
  static SoyBean_Factory_type SoyBean_Factory(SoyBean_Factory_type inject = nullptr);
};

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
