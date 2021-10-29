#ifndef MILK_LAUNCH_H_
#define MILK_LAUNCH_H_

#include <string_view>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <functional>

#include <milkpowder.h>

#include "util.h"

namespace Milk {

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
    MilkTea::Logger::Level level = MilkTea::Logger::Level::ASSERT;
    if (itr == args.end()) {
      std::cerr << "milk dump --log: need log level" << std::endl;
      return false;
    } else if (*itr == "d" || *itr == "debug") {
      level = MilkTea::Logger::Level::DEBUG;
    } else if (*itr == "i" || *itr == "info") {
      level = MilkTea::Logger::Level::INFO;
    } else if (*itr == "w" || *itr == "warn") {
      level = MilkTea::Logger::Level::WARN;
    } else if (*itr == "e" || *itr == "error") {
      level = MilkTea::Logger::Level::ERROR;
    }
    if (level == MilkTea::Logger::Level::ASSERT) {
      std::cerr << "milk dump --log: invalid log level: " << *itr << std::endl;
      return false;
    }
    MilkTea_Logger_Init(Inject::Logger()(MilkTea::BaseLogger::RawLevel(level)));
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
  std::map<std::string_view, std::function<bool(std::list<std::string_view>::iterator &, std::list<std::string_view> &)>> callbacks_;
  std::function<void()> show_help_;
};

} // namespace Milk

#endif // ifndef MILK_LAUNCH_H_
