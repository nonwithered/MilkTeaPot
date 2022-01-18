#ifndef MILK_CONTROL_H_
#define MILK_CONTROL_H_

#include <string_view>
#include <functional>
#include <map>
#include <list>
#include <memory>

#include "context.h"
#include "printer.h"
#include "util.h"

namespace Milk {
template<typename container_type>
class Cursor final {
  static constexpr char TAG[] = "Milk::Cursor";
  using value_type = typename container_type::value_type;
  using iterator = typename container_type::iterator;
public:
  Cursor(container_type &container, iterator &cursor)
  : container_(container),
    cursor_(cursor) {}
  const value_type &operator*() const {
    if (!operator bool()) {
      MilkTea_throw(LogicError, "try to get but no more");
    }
    return *cursor_;
  }
  bool operator++() {
    if (!operator bool()) {
      return false;
    }
    cursor_ = container_.erase(cursor_);
    return true;
  }
  operator bool() const {
    return cursor_ != container_.end();
  }
private:
  container_type &container_;
  iterator &cursor_;
};

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
      auto cursor = cursor_type(args, i);
      if (!it->second(cursor)) {
        return;
      }
    }
    if (version_) {
      Out() << "version: " << kVersion << End();
    }
    if (help_) {
      Out() << usage_ << End();
    }
    Context().SetLogLevel(level_);
    Main(args);
  }
 protected:
  using cursor_type = Cursor<std::list<std::string_view>>;
  static constexpr std::nullptr_t End() {
    return nullptr;
  }
  static constexpr std::string_view Line() {
    return "\n";
  }
  virtual void Main(std::list<std::string_view> &) = 0;
  BaseController(BaseContext &context, std::string usage) :
    context_(context),
    out_(context.GetPrinterOut()),
    err_(context.GetPrinterErr()),
    usage_(std::move(usage)) {
    Config(&self_type::help_, {
      "-h",
      "--help",
    });
    Config(&self_type::SetLogLevel, {
      "--log",
    });
    Config(&self_type::version_, {
      "-v",
      "--version",
    });
  }
  template<typename Controller>
  void Config(bool Controller::* f, std::initializer_list<std::string_view> args) {
    Config(args, [this, f](auto &) -> bool {
      dynamic_cast<Controller *>(this)->*f = true;
      return true;
    });
  }
  template<typename Controller>
  void Config(void (Controller::* f)(), std::initializer_list<std::string_view> args) {
    Config(args, [this, f](auto &) -> bool {
      std::bind(f, dynamic_cast<Controller *>(this))();
      return true;
    });
  }
  template<typename Controller>
  void Config(bool (Controller::* f)(cursor_type &), std::initializer_list<std::string_view> args) {
    Config(args, [this, f](auto &cursor) -> auto {
      return (dynamic_cast<Controller *>(this)->*f)(cursor);
    });
  }
  BaseContext &Context() {
    return context_;
  }
  BufferPrinter &Out() {
    return out_;
  }
  BufferPrinter &Err() {
    return err_;
  }
 private:
  void Config(const std::initializer_list<std::string_view> &args, std::function<bool(cursor_type &)> f) {
    for (auto it : args) {
      callbacks_[it] = f;
    }
  }
  bool SetLogLevel(cursor_type &cursor) {
    if (!cursor) {
      Err() << "milk dump --log: need log level" << End();
      return false;
    } else if (*cursor == "d" || *cursor == "debug") {
      level_ = MilkTea::Logger::Level::DEBUG;
    } else if (*cursor == "i" || *cursor == "info") {
      level_ = MilkTea::Logger::Level::INFO;
    } else if (*cursor == "w" || *cursor == "warn") {
      level_ = MilkTea::Logger::Level::WARN;
    } else if (*cursor == "e" || *cursor == "error") {
      level_ = MilkTea::Logger::Level::ERROR;
    } else {
      Err() << "milk dump --log: invalid log level: " << *cursor << End();
      return false;
    }
    ++cursor;
    return true;
  }
  BaseContext &context_;
  BufferPrinter out_;
  BufferPrinter err_;
  const std::string usage_;
  std::map<std::string_view, std::function<bool(cursor_type &)>> callbacks_;
  bool version_ = false;
  bool help_ = false;
  MilkTea::Logger::Level level_ = MilkTea::Logger::Level::ASSERT;
};

} // namespace Milk

#endif // ifndef MILK_CONTROL_H_
