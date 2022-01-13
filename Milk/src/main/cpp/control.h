#ifndef MILK_CONTROL_H_
#define MILK_CONTROL_H_

#include <string_view>
#include <functional>
#include <map>
#include <list>
#include <memory>

#include "util.h"
#include "config.h"

namespace Milk {

class BaseController {
  static constexpr char TAG[] = "Milk::BaseController";
  using self_type = BaseController;
 public:
  static constexpr auto kName = "";
  static constexpr auto kUsage = "";
  void Launch(std::list<std::string_view> &args) {
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
    Main(args);
  }
 protected:
  virtual void Main(std::list<std::string_view> &) = 0;
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
  BaseController(std::string help_text) :
    help_text_(std::move(help_text)),
    help_(false),
    version_(false),
    callbacks_() {
    Config(&self_type::ShowHelp, {
      "-h",
      "--help",
    });
    Config(&self_type::InitLog, {
      "--log",
    });
    Config(&self_type::ShowVersion, {
      "-v",
      "--version",
    });
  }
  template<typename Controller>
  void Config(void (Controller::* f)(), std::initializer_list<std::string_view> args) {
    auto callback = [this, f](auto &) -> bool {
      std::bind(f, dynamic_cast<Controller *>(this))();
      return true;
    };
    for (auto it : args) {
      callbacks_[it] = callback;
    }
  }
  template<typename Controller>
  void Config(bool (Controller::* f)(ArgsCursor &), std::initializer_list<std::string_view> args) {
    auto callback = [this, f](auto &cursor) -> bool {
      return (dynamic_cast<Controller *>(this)->*f)(cursor);
    };
    for (auto it : args) {
      callbacks_[it] = callback;
    }
  }
 private:
  void ShowVersion() {
    if (version_) {
      return;
    } else {
      version_ = true;
    }
    std::cout << "version: " << kVersion << "\n" << std::endl;
  }
  void ShowHelp() {
    if (help_) {
      return;
    } else {
      help_ = true;
    }
    std::cout << help_text_ << "\n" << std::endl;
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
  const std::string help_text_;
  bool help_;
  bool version_;
  std::map<std::string_view, std::function<bool(ArgsCursor &)>> callbacks_;
};

} // namespace Milk

#endif // ifndef MILK_CONTROL_H_
