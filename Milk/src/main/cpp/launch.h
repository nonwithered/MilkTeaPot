#ifndef MILK_LAUNCH_H_
#define MILK_LAUNCH_H_

#include <string_view>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <initializer_list>

#include <milkpowder.h>

#include "util.h"
#include "config.h"

namespace Milk {

class ArgsCursor final {
  using container = std::list<std::string_view>;
  using iterator = container::iterator;
 public:
  ArgsCursor(container &container, iterator &cursor) : container_(container), cursor_(cursor) {}
  std::string_view operator*() const {
    if (!operator bool()) {
      return nullptr;
    }
    return *cursor_;
  }
  bool operator++() {
    if (cursor_ == container_.end()) {
      return false;
    }
    cursor_ = container_.erase(cursor_);
    return true;
  }
  operator bool() const {
    return cursor_ != container_.end();
  }
 private:
  container &container_;
  iterator &cursor_;
};

class Command {
  static constexpr char TAG[] = "Milk::Command";
  using self_type = Command;
 public:
  static void LaunchMain(std::list<std::string_view> args, std::initializer_list<std::unique_ptr<Milk::Command>> arr) {
    auto i = arr.begin();
    if (!args.empty()) {
      while (i != arr.end()) {
        if ((*i)->Name() == args.front()) {
          break;
        }
        ++i;
      }
      if (i != arr.end()) {
        args.pop_front();
      } else {
        i = arr.begin();
      }
    }
    (*i)->show_help_ = [&arr]() -> void {
      for (auto &it : arr) {
        std::cerr << it->Usage() << std::endl;
      }
    };
    (*i)->LaunchInternal(args);
  }
  virtual ~Command() = default;
 protected:
  Command()
  : help_(false),
    version_(false) {
    Callback(&self_type::ShowHelp, {
      "-h",
      "--help",
    });
    Callback(&self_type::InitLog, {
      "--log",
    });
    Callback(&self_type::ShowVersion, {
      "-v",
      "--version",
    });
  }
  static void Version() {
    std::cout << "version: " << kVersion << "\n" << std::endl;
  }
  virtual void Launch(std::list<std::string_view> &) = 0;
  virtual std::string_view Usage() const = 0;
  virtual std::string_view Name() const = 0;
  virtual void Help() const {
    show_help_();
  }
  template<typename CMD>
  void Callback(void (CMD::* f)(), std::initializer_list<std::string_view> args) {
    auto callback = [this, f](auto &) -> bool {
      std::bind(f, dynamic_cast<CMD *>(this))();
      return true;
    };
    for (auto it : args) {
      callbacks_[it] = callback;
    }
  }
  template<typename CMD>
  void Callback(bool (CMD::* f)(ArgsCursor &), std::initializer_list<std::string_view> args) {
    auto callback = [this, f](auto &cursor) -> bool {
      return (dynamic_cast<CMD *>(this)->*f)(cursor);
    };
    for (auto it : args) {
      callbacks_[it] = callback;
    }
  }
 private:
  void LaunchInternal(std::list<std::string_view> &args) {
    for (auto i = args.begin(); i != args.end(); ) {
      auto it = callbacks_.find(*i);
      if (it == callbacks_.end()) {
        ++i;
        continue;
      }
      i = args.erase(i);
      auto cursor = ArgsCursor(args, i);
      if (!it->second(cursor)) {
        ShowHelp();
        return;
      }
    }
    if (args.size() == 0) {
      std::cerr << "milk " << Name() << ": no input files" << std::endl;
      return;
    }
    Launch(args);
  }
  void ShowHelp() {
    if (help_) {
      return;
    } else {
      help_ = true;
    }
    Help();
  }
  void ShowVersion() {
    if (version_) {
      return;
    } else {
      version_ = true;
    }
    Version();
  }
  bool InitLog(ArgsCursor &cursor) {
    MilkTea::Logger::Level level = MilkTea::Logger::Level::ASSERT;
    if (!cursor) {
      std::cerr << "milk dump --log: need log level" << std::endl;
      return false;
    } else if (*cursor == "d" || *cursor == "debug") {
      level = MilkTea::Logger::Level::DEBUG;
    } else if (*cursor == "i" || *cursor == "info") {
      level = MilkTea::Logger::Level::INFO;
    } else if (*cursor == "w" || *cursor == "warn") {
      level = MilkTea::Logger::Level::WARN;
    } else if (*cursor == "e" || *cursor == "error") {
      level = MilkTea::Logger::Level::ERROR;
    }
    if (level == MilkTea::Logger::Level::ASSERT) {
      std::cerr << "milk dump --log: invalid log level: " << *cursor << std::endl;
      return false;
    }
    MilkTea::Logger::Config(ConfigWrapper::Instance().make_logger(level));
    ++cursor;
    return true;
  }
  std::map<std::string_view, std::function<bool(ArgsCursor &)>> callbacks_;
  std::function<void()> show_help_;
  bool help_;
  bool version_;
};

} // namespace Milk

#endif // ifndef MILK_LAUNCH_H_
