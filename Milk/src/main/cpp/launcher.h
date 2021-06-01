#ifndef MILK_LAUNCH_H_
#define MILK_LAUNCH_H_

#include <ctime>
#include <iostream>
#include <string_view>
#include <list>
#include <map>
#include <memory>
#include <functional>
#include <string>

#include <milkpowder.h>

inline std::string LogTime() {
  time_t t = time(nullptr);
  return ctime(&t);
}

template<MilkPowder_Log_Level_t level>
inline void LogPrint(void *, const char *tag, const char *msg);

template<>
inline void LogPrint<MilkPowder_Log_Level_t::DEBUG>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/DEBUG: " << tag << ": " << msg << std::endl;
}

template<>
inline void LogPrint<MilkPowder_Log_Level_t::INFO>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/INFO: " << tag << ": " << msg << std::endl;
}

template<>
inline void LogPrint<MilkPowder_Log_Level_t::WARN>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/WARN: " << tag << ": " << msg << std::endl;
}

template<>
inline void LogPrint<MilkPowder_Log_Level_t::ERROR>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/ERROR: " << tag << ": " << msg << std::endl;
}

inline void LogInitLevel(MilkPowder_Log_Level_t level) {
  MilkPowder_Log_Config_t config = {
    .obj = nullptr,
    .debug = LogPrint<MilkPowder_Log_Level_t::DEBUG>,
    .info = LogPrint<MilkPowder_Log_Level_t::INFO>,
    .warn = LogPrint<MilkPowder_Log_Level_t::WARN>,
    .error = LogPrint<MilkPowder_Log_Level_t::ERROR>,
    .level = level
  };
  MilkPowder_Log_Init(config);
}

namespace Milk {

class Launcher final {
 public:
  Launcher(int argc, char *argv[])
    : args_() {
    for (int i = 1; i < argc; ++i) {
      args_.emplace_back(argv[i]);
    }
  }
  void Launch() {
    void (*launch)(std::list<std::string_view> &) = nullptr;
    if (args_.front() == "codec") {
      launch = LaunchCodec;
    } else if (args_.front() == "probe") {
      launch = LaunchProbe;
    } else if (args_.front() == "play") {
      launch = LaunchPlay;
    }
    if (launch != nullptr) {
      args_.pop_front();
      launch(args_);
    } else {
      std::cerr << UsageCodec() << std::endl;
      std::cerr << UsageProbe() << std::endl;
      std::cerr << UsagePlay() << std::endl;
    }
  }
 private:
  std::list<std::string_view> args_;
  static void LaunchCodec(std::list<std::string_view> &);
  static void LaunchProbe(std::list<std::string_view> &);
  static void LaunchPlay(std::list<std::string_view> &);
  static std::string_view UsageCodec();
  static std::string_view UsageProbe();
  static std::string_view UsagePlay();
};

} // namespace Milk

#endif // ifndef MILK_LAUNCH_H_
