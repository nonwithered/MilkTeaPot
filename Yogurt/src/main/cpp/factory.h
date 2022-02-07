#ifndef YOGURT_FACTORY_H_
#define YOGURT_FACTORY_H_

#include "handle.h"
#include "record.h"

namespace Yogurt {

class FactoryImpl final : public SoyBean::BaseFactory {
  static constexpr char TAG[] = "Yogurt::FactoryImpl";
 public:
  explicit FactoryImpl(std::weak_ptr<RecorderImpl> recorder, uint32_t tempo)
  : recorder_(std::move(recorder)),
    tempo_(tempo) {}
  FactoryImpl(FactoryImpl &&another) : FactoryImpl(std::move(another.recorder_), another.tempo_) {}
  BaseFactory &Move() && final {
    return *new FactoryImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  SoyBean_Handle_t Create() final {
    auto recorder = recorder_.lock();
    if (!recorder) {
      MilkTea_throwf(LogicError, "create handle but recorder is dead");
    }
    if (auto state = recorder->state(); state != RecorderState::PAUSE) {
      MilkTea_throwf(LogicError, "create handle but recorder is %s", StateName(state));
    }
    return HandleImpl(recorder->Append(TickClock(recorder->division(), tempo_))).ToRawType();
  }
 private:
  const std::weak_ptr<RecorderImpl> recorder_;
  const uint32_t tempo_;
  MilkTea_NonCopy(FactoryImpl)
  MilkTea_NonMoveAssign(FactoryImpl)
};

} // namespace Yogurt

#endif // ifndef YOGURT_FACTORY_H_
