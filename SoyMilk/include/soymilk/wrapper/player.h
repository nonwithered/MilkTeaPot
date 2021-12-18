#ifndef LIB_SOYMILK_COMMON_PLAYER_H_
#define LIB_SOYMILK_COMMON_PLAYER_H_

#include <soymilk/common.h>

namespace SoyMilk {

class PlayerWrapper final {
  static constexpr char TAG[] = "SoyMilk::PlayerWrapper";
  PlayerWrapper(SoyMilk_Player_t *self = nullptr) : self_(self) {}
  PlayerWrapper(PlayerWrapper &&another) : PlayerWrapper() {
    std::swap(self_, another.self_);
  }
 private:
  SoyMilk_Player_t *self_;
  MilkTea_NonCopy(PlayerWrapper)
  MilkTea_NonMoveAssign(PlayerWrapper)
};

} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_PLAYER_H_
