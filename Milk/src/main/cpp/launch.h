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

template<MilkPowder_LogLevel_t level>
void LogPrint(void *, const char *tag, const char *msg);

template<>
void LogPrint<MilkPowder_LogLevel_t::DEBUG>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/DEBUG: " << tag << ": " << msg << std::endl;
}

template<>
void LogPrint<MilkPowder_LogLevel_t::INFO>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/INFO: " << tag << ": " << msg << std::endl;
}

template<>
void LogPrint<MilkPowder_LogLevel_t::WARN>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/WARN: " << tag << ": " << msg << std::endl;
}

template<>
void LogPrint<MilkPowder_LogLevel_t::ERROR>(void *, const char *tag, const char *msg) {
  std::cerr << LogTime() << "/ERROR: " << tag << ": " << msg << std::endl;
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
  bool InitLog(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    MilkPowder_LogLevel_t level = MilkPowder_LogLevel_t::ASSERT;
    if (itr == args.end()) {
      std::cerr << "milk dump --log: need log level" << std::endl;
      return false;
    } else if (*itr == "d" || *itr == "debug") {
      level = MilkPowder_LogLevel_t::DEBUG;
    } else if (*itr == "i" || *itr == "info") {
      level = MilkPowder_LogLevel_t::INFO;
    } else if (*itr == "w" || *itr == "warn") {
      level = MilkPowder_LogLevel_t::WARN;
    } else if (*itr == "e" || *itr == "error") {
      level = MilkPowder_LogLevel_t::ERROR;
    }
    if (level == MilkPowder_LogLevel_t::ASSERT) {
      std::cerr << "milk dump --log: invalid log level: " << *itr << std::endl;
      return false;
    }
    LogInitLevel(level);
    itr = args.erase(itr);
    return true;
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
  static void LogInitLevel(MilkPowder_LogLevel_t level) {
    MilkPowder_Logger_t config = {
      .obj = nullptr,
      .debug = LogPrint<MilkPowder_LogLevel_t::DEBUG>,
      .info = LogPrint<MilkPowder_LogLevel_t::INFO>,
      .warn = LogPrint<MilkPowder_LogLevel_t::WARN>,
      .error = LogPrint<MilkPowder_LogLevel_t::ERROR>,
      .level = level
    };
    MilkPowder_Log_Init(config);
  }
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  std::function<void()> show_help_;
};

} // namespace Milk

#endif // ifndef MILK_LAUNCH_H_
