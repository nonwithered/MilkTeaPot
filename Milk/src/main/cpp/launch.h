#ifndef MILK_LAUNCH_H_
#define MILK_LAUNCH_H_

#include <ctime>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <list>
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <sstream>

#include <milkpowder.h>

#include "util.h"

namespace Milk {

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

inline const char *ErrName(MilkPowder_Err_t type) {
  switch (type) {
    case MilkPowder_Err_t::Nil: return "Nil";
    case MilkPowder_Err_t::Assertion: return "Assertion";
    case MilkPowder_Err_t::NullPointer: return "NullPointer";
    case MilkPowder_Err_t::Unsupported: return "Unsupported";
    case MilkPowder_Err_t::EndOfFile: return "EndOfFile";
    case MilkPowder_Err_t::InvalidParam: return "InvalidParam";
    case MilkPowder_Err_t::LogicError: return "LogicError";
    default: return "Unknown";
  }
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

class Command {
 public:
  static void LaunchMain(std::list<std::string_view> &args, const std::vector<std::unique_ptr<Milk::Command>> &arr) {
    auto itr = arr.begin();
    if (!args.empty()) {
      while (itr != arr.end()) {
        if ((*itr)->Name() == args.front()) {
          break;
        }
        ++itr;
      }
      if (itr != arr.end()) {
        args.pop_front();
      } else {
        itr = arr.begin();
      }
    }
    (*itr)->show_help_ = [&arr]() -> void {
      for (auto &it : arr) {
        std::cerr << it->Usage() << std::endl;
      }
    };
    (*itr)->LaunchInternal(args);
  }
  virtual ~Command() = default;
 protected:
  static void ShowVersion() {
    std::cout << "version: " << kVersion << "\n" << std::endl;
  }
  void ShowHelp() {
    show_help_();
  }
  virtual void Launch(std::list<std::string_view> &) = 0;
  virtual std::string_view Usage() const = 0;
  virtual std::string_view Name() const = 0;
  template<typename CMD>
  void Callback(std::string_view k, bool (CMD::* v)(std::list<std::string_view>::iterator &, std::list<std::string_view> &)) {
    callbacks_[k] = [this, v](auto &itr, auto &args) -> bool {
      return (dynamic_cast<CMD *>(this)->*v)(itr, args);
    };
  }
 private:
  void LaunchInternal(std::list<std::string_view> &args) {
    for (auto itr = args.begin(); itr != args.end(); ) {
      auto it = callbacks_.find(*itr);
      if (it != callbacks_.end()) {
        itr = args.erase(itr);
        if (!it->second(itr, args)) {
          ShowHelp();
          return;
        }
      } else {
        ++itr;
      }
    }
    if (args.size() == 0) {
      std::cerr << "milk " << Name() << ": no input files" << std::endl;
      return;
    }
    Launch(args);
  }
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  std::function<void()> show_help_;
};

} // namespace Milk

#endif // ifndef MILK_LAUNCH_H_
