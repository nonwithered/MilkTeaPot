#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include <soymilk.h>

#include "launch.h"

#include <soybean_windows.h>

namespace Milk {

class Play final : public Command {
 public:
  Play() : Command() {
    Callback("--log", &Play::InitLog);
  }
 protected:
  void Launch(std::list<std::string_view> &) final {
    SoyBean_Handle_t h;
    MilkTea_Exception_t e;
    e = SoyBean_Handle_Create(&h, SoyBean_Windows_Factory_Instance(), SoyBean_Windows_Factory_Callback());
    std::cerr << e << std::endl;
    e = SoyBean_Handle_NoteOn(h, 0, 0x45, 0x7f);
    std::cerr << e << std::endl;
    std::cerr << MilkTea_Exception_What() << std::endl;
    while (true) ;
  }
  std::string_view Usage() const final {
    return
"Usage: milk play\n"
    ;
  }
  std::string_view Name() const final {
    return "play";
  }
};

} // namespace Milk

#endif // ifndef MILK_PLAY_H_
