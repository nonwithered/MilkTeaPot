#ifndef YOGURT_HANDLE_H_
#define YOGURT_HANDLE_H_

#include <functional>

#include <milkpowder.h>

#include "clock.h"

namespace Yogurt {

class HandleImpl final : public SoyBean::BaseHandle {
 public:
  explicit HandleImpl(consumer_type consumer) : consumer_(consumer) {}
  HandleImpl(HandleImpl &&another) : HandleImpl(std::move(another.consumer_)) {}
  BaseHandle &Move() && final {
    return *new HandleImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void SendMessage(uint8_t type, uint8_t arg0, uint8_t arg1) final {
    Consume(make_event(type, arg0, arg1));
  }
 private:
  static MilkPowder::EventMutableWrapper make_event(uint8_t type, uint8_t arg0, uint8_t arg1) {
    return MilkPowder::EventMutableWrapper::Make(0, type, { arg0, arg1, });
  }
  void Consume(MilkPowder::EventMutableWrapper event) {
    consumer_(std::move(event));
  }
  void Frame(uint8_t type, uint8_t channel, uint8_t arg0, uint8_t arg1 = 0) {
    channel &= 0x0f;
    Consume(make_event(type | channel, arg0, arg1));
  }
  const consumer_type consumer_;
  MilkTea_NonCopy(HandleImpl)
  MilkTea_NonMoveAssign(HandleImpl)
};

} // namespace Yogurt

#endif // ifndef YOGURT_HANDLE_H_
