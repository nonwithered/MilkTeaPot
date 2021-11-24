#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include <thread>
#include <chrono>

#include <soymilk.h>

#include "launch.h"

namespace Milk {

class Play final : public Command {
 public:
  Play() : Command() {
    Callback("-h", &Play::ShowHelp);
    Callback("--help", &Play::ShowHelp);
    Callback("--log", &Play::InitLog);
  }
 protected:
  void Launch(std::list<std::string_view> &) final {
    auto handle = GetFactory()->Create();
    handle->NoteOn(0, 0x45, 0x7f);
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
  std::string_view Usage() const final {
    return
"Usage: milk play\n"
"  -h, --help\n"
"    print this help message\n"
"  --log {d, i, w, e, debug, info, warn, error}\n"
"    init log level, or no log\n"
    ;
  }
  std::string_view Name() const final {
    return "play";
  }
 private:
  static std::unique_ptr<SoyBean::FactoryWrapper> GetFactory() {
    SoyBean_Factory_t factory{};
    MilkTea_panic(Inject::SoyBean_Factory()(&factory));
    return SoyBean::FactoryWrapper::Make(std::move(factory));
  }
  bool help_;
  bool ShowHelp(std::list<std::string_view>::iterator &, std::list<std::string_view> &) {
    if (help_) {
      return true;
    } else {
      help_ = true;
    }
    std::cerr << Usage() << std::endl;
    return true;
  }
  static constexpr char TAG[] = "Milk#Play";
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
