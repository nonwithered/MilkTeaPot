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

template<typename Controller>
struct Mode final {
  using controller_type = Controller;
  static std::string_view Name() {
    return controller_type::kName;
  }
  static void Start(args_type &args, Milk::ContextWrapper &context) {
    controller_type(context).Start(args);
  }
};

template<typename Controller>
class BaseController {
  static constexpr char TAG[] = "Milk::BaseController";
 public:
  void Start(args_type &args) {
    if (!Config(Cocoa::Pipeline(args, dynamic_cast<self_type &>(*this))).Start()) {
      return;
    }
    Prepare();
    Main(args);
  }
 protected:
  using self_type = Controller;
  using super_type = BaseController<self_type>;
  using value_type = args_type::value_type;
  using cursor_type = Cocoa::Cursor<args_type>;
  using pipeline_type = Cocoa::Pipeline<args_type, self_type>;
  virtual void Main(args_type &) = 0;
  virtual pipeline_type Config(pipeline_type &&pipeline) {
    return std::move(pipeline)
      .Append({
          "-h",
          "--help",
        }, &self_type::help_, true)
      .Append({
          "-v",
          "--version",
        }, &self_type::version_, true)
      .Append({
          "--log",
        }, &self_type::level_,
        [this]() {
          Err() << Tip() << "--log: need log level" << End();
        },
        [this](auto &it) {
          Err() << Tip() << "--log: invalid log level: " << it << End();
        }, {
          { MilkTea::Logger::Level::DEBUG, { "d", "debug" } },
          { MilkTea::Logger::Level::INFO, { "i", "info" } },
          { MilkTea::Logger::Level::WARN, { "w", "warn" } },
          { MilkTea::Logger::Level::ERROR, { "e", "error" } },
        })
      ;
  }
  static constexpr std::nullptr_t End() {
    return nullptr;
  }
  static constexpr std::string_view Line() {
    return "\n";
  }
  BaseController(ContextWrapper &context)
  : context_(context),
    out_(context.GetOutWriter()),
    err_(context.GetErrWriter()) {}
  ContextWrapper &Context() {
    return context_;
  }
  PrinterImpl &Out() {
    return out_;
  }
  PrinterImpl &Err() {
    return err_;
  }
  std::string_view Name() const {
    return self_type::kName;
  }
  std::string_view Usage() const {
    return self_type::kUsage;
  }
  std::string Tip() const {
    return std::string(kName) + " " + Name().data() + ": ";
  }
 private:
  void Prepare() {
    if (version_) {
      Out() << "version: " << kVersion << End();
    }
    if (help_) {
      Out() << Usage() << End();
    }
    Context().SetLogLevel(level_);
  }
  ContextWrapper &context_;
  PrinterImpl out_;
  PrinterImpl err_;
  bool version_ = false;
  bool help_ = false;
  MilkTea::Logger::Level level_ = MilkTea::Logger::Level::ASSERT;
  MilkTea_NonCopy(BaseController);
  MilkTea_NonMove(BaseController);
};

} // namespace Milk

#endif // ifndef MILK_CONTROL_H_
