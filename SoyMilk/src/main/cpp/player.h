#ifndef SOYMILK_PLAYER_H_
#define SOYMILK_PLAYER_H_

#include <milkpowder.h>

namespace SoyMilk {

class Player final {
 public:
  explicit Player(const MilkTea::Timer &);
  ~Player();
  void Init();
 private:
};

} // namespace SoyBean_Windows

#endif // ifndef SOYMILK_PLAYER_H_
