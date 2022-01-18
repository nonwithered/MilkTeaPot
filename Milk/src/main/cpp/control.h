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
    Config(&self_type::level_,
      "milk dump --log: need log level",
      "milk dump --log: invalid log level: ",
      {
        { "d", MilkTea::Logger::Level::DEBUG },
        { "debug", MilkTea::Logger::Level::DEBUG },
        { "i", MilkTea::Logger::Level::INFO },
        { "info", MilkTea::Logger::Level::INFO },
        { "w", MilkTea::Logger::Level::WARN },
        { "warn", MilkTea::Logger::Level::WARN },
        { "e", MilkTea::Logger::Level::ERROR },
        { "error", MilkTea::Logger::Level::ERROR },
      }, {
        "--log",
      });
    Config(&self_type::version_, {
        "-v",
        "--version",
      });
  }
  template<typename Controller>
  void Config(bool Controller::* member,
      std::initializer_list<std::string_view> args) {
    Config(args, [this, member](auto &) -> bool {
      dynamic_cast<Controller *>(this)->*member = true;
      return true;
    });
  }
  template<typename Controller, typename T>
  void Config(T Controller::* member,
      std::string_view msg_none, std::string_view msg_invalid,
      std::map<std::string_view, T> value_map,
      std::initializer_list<std::string_view> args) {
    Config(args, [this, member, msg_none, msg_invalid, value_map](auto &cursor) -> bool {
      if (!cursor) {
        Err() << msg_none << End();
        return false;
      }
      auto &value = *cursor;
      auto it = value_map.find(value);
      if (it == value_map.end()) {
        Err() << msg_invalid << value << End();
        return false;
      }
      dynamic_cast<Controller *>(this)->*member = it->second;
      ++cursor;
      return true;
    });
  }
  template<typename Controller>
  void Config(void (Controller::* member)(),
      std::initializer_list<std::string_view> args) {
    Config(args, [this, member](auto &) -> bool {
      std::bind(member, dynamic_cast<Controller *>(this))();
      return true;
    });
  }
  template<typename Controller>
  void Config(bool (Controller::* member)(cursor_type &),
      std::initializer_list<std::string_view> args) {
    Config(args, [this, member](auto &cursor) -> auto {
      return (dynamic_cast<Controller *>(this)->*member)(cursor);
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
