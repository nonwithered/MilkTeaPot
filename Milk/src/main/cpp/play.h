#ifndef MILK_PLAY_H_
#define MILK_PLAY_H_

#include "launch.h"

namespace Milk {

class Play final : public Command {
 public:
  Play() : Command() {
  }
 protected:
  void Launch(std::list<std::string_view> &) final {
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
