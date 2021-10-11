#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include <soymilk.h>

#include "launch.h"

#include <soybean_windows.h>

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
    SoyBean_Handle_t h;
    MilkTea_Exception_t e;
    SoyBean_Factory_t factory;
    e = SoyBean_Windows_Factory_Create(&factory, 0, 0, 0, 0);
    std::cerr << e << std::endl; std::cerr << MilkTea_Exception_What() << std::endl;
    e = SoyBean_Handle_Create(&h, factory);
    std::cerr << e << std::endl; std::cerr << MilkTea_Exception_What() << std::endl;
    e = SoyBean_Factory_Destroy(factory);
    std::cerr << e << std::endl; std::cerr << MilkTea_Exception_What() << std::endl;
    e = SoyBean_Handle_NoteOn(h, 0, 0x45, 0x7f);
    std::cerr << e << std::endl; std::cerr << MilkTea_Exception_What() << std::endl;
    while (true) ;
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
  bool help_;
  bool ShowHelp(std::list<std::string_view>::iterator &itr, std::list<std::string_view> &args) {
    if (help_) {
      return true;
    } else {
      help_ = true;
    }
    std::cerr << Usage() << std::endl;
    return true;
  }
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
