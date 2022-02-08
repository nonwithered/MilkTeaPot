#ifndef LIB_SOYMILK_COMMON_PLAYER_H_
#define LIB_SOYMILK_COMMON_PLAYER_H_

#include <soymilk/common.h>

namespace SoyMilk {

class PlayerWrapper final {
  static constexpr char TAG[] = "SoyMilk::PlayerWrapper";
  using raw_type = SoyMilk_Player_t;
  using State = Player::State;
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  PlayerWrapper(raw_type *self = nullptr) : self_(self) {}
  PlayerWrapper(PlayerWrapper &&another) : PlayerWrapper(another.release()) {}
  PlayerWrapper(BaseRenderer &&renderer, TeaPot::Executor::executor_type executor, TeaPot::TimerWorkerWeakWrapper timer) : PlayerWrapper() {
    MilkTea_invoke_panic(SoyMilk_Player_Create, &self_, std::move(renderer).ToRawType(), TeaPot::Executor::ToRawType(executor), timer.release());
  }
  ~PlayerWrapper() {
    auto self = release();
    if (self == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyMilk_Player_Destroy, self);
  }
  State GetState() {
    SoyMilk_Player_State_t state = SoyMilk_Player_State_t::SoyMilk_Player_State_INIT;
    MilkTea_invoke_panic(SoyMilk_Player_GetState, get(), &state);
    return Player::FromRawType(state);
  }
  void Prepare(MilkPowder::MidiConstWrapper midi) {
    MilkTea_invoke_panic(SoyMilk_Player_Prepare, get(), midi.get());
  }
  void Start() {
    MilkTea_invoke_panic(SoyMilk_Player_Start, get());
  }
  void Pause() {
    MilkTea_invoke_panic(SoyMilk_Player_Pause, get());
  }
  void Seek(duration_type time) {
    MilkTea_invoke_panic(SoyMilk_Player_Seek, get(), time.count());
  }
  void Resume() {
    MilkTea_invoke_panic(SoyMilk_Player_Resume, get());
  }
  void Stop() {
    MilkTea_invoke_panic(SoyMilk_Player_Stop, get());
  }
  void Reset() {
    MilkTea_invoke_panic(SoyMilk_Player_Reset, get());
  }
  raw_type *get() const {
    return self_;
  }
  raw_type *release() {
    return reset(nullptr);
  }
  raw_type *reset(raw_type *another) {
    std::swap(another, self_);
    return another;
  }
 private:
  raw_type *self_;
  MilkTea_NonCopy(PlayerWrapper)
  MilkTea_NonMoveAssign(PlayerWrapper)
};

} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_PLAYER_H_
