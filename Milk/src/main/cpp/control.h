#ifndef MILK_CONTROL_H_
#define MILK_CONTROL_H_

#include <string_view>
#include <functional>
#include <map>
#include <list>
#include <memory>

#include <cocoa.h>

#include "context.h"
#include "printer.h"
#include "util.h"

namespace Milk {

class BaseController {
  static constexpr char TAG[] = "Milk::BaseController";
  using self_type = BaseController;
 public:
  static constexpr auto kName = "";
  static constexpr auto kUsage = "";
  virtual void Main(std::list<std::string_view> &) {
    if (version_) {
      Out() << "version: " << kVersion << End();
    }
    if (help_) {
      Out() << usage_ << End();
    }
    Context().SetLogLevel(level_);
  }
 protected:
  using container_type = std::list<std::string_view>;
  using cursor_type = Cocoa::Cursor<container_type>;
  static constexpr std::nullptr_t End() {
    return nullptr;
  }
  static constexpr std::string_view Line() {
    return "\n";
  }
  template<typename attribute_type, typename container_type>
  Cocoa::Pipeline<attribute_type, container_type> Config(Cocoa::Pipeline<attribute_type, container_type> &&pipeline) {
    return std::forward<Cocoa::Pipeline<attribute_type, container_type>>(pipeline)
      .Append({
          "-h",
          "--help",
        }, &attribute_type::help_, true)
      .Append({
          "-v",
          "--version",
        }, &attribute_type::version_, true)
      .Append({
          "--log",
        }, &attribute_type::level_,
        [this](auto &cmd) {
          Err() << "milk " << cmd << ": need log level" << End();
        },
        [this](auto &cmd, auto &it) {
          Err() << "milk " << cmd << ": invalid log level: " << it << End();
        }, {
          { MilkTea::Logger::Level::DEBUG, { "d", "debug" } },
          { MilkTea::Logger::Level::INFO, { "i", "info" } },
          { MilkTea::Logger::Level::WARN, { "w", "warn" } },
          { MilkTea::Logger::Level::ERROR, { "e", "error" } },
        });
  }
  BaseController(BaseContext &context, std::string_view usage)
  : context_(context),
    usage_(std::move(usage)),
    out_(context.GetPrinterOut()),
    err_(context.GetPrinterErr()) {}
  BaseContext &Context() {
    return context_;
  }
  BufferPrinter &Out() {
    return out_;
  }
  BufferPrinter &Err() {
    return err_;
  }
  bool version_ = false;
  bool help_ = false;
  MilkTea::Logger::Level level_ = MilkTea::Logger::Level::ASSERT;
 private:
  BaseContext &context_;
  std::string usage_;
  BufferPrinter out_;
  BufferPrinter err_;
};

} // namespace Milk

#endif // ifndef MILK_CONTROL_H_
