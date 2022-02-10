#ifndef YOGURT_HANDLE_H_
#define YOGURT_HANDLE_H_

#include <functional>

#include <soybean.h>

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
  void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) final {
    Frame(0x80, channel, note, pressure);
  }
  void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) final {
    Frame(0x90, channel, note, pressure);
  }
  void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) final {
    Frame(0xa0, channel, note, pressure);
  }
  void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) final {
    Frame(0xb0, channel, control, argument);
  }
  void ProgramChange(uint8_t channel, uint8_t program) final {
    Frame(0xc0, channel, program);
  }
  void ChannelPressure(uint8_t channel, uint8_t pressure) final {
    Frame(0xd0, channel, pressure);
  }
  void PitchBend(uint8_t channel, uint8_t low, uint8_t height) final {
    Frame(0xe0, channel, low, height);
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
