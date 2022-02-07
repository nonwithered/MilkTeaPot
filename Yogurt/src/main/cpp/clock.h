#ifndef YOGURT_CLOCK_H_
#define YOGURT_CLOCK_H_

#include <chrono>

#include <milkpowder.h>

namespace Yogurt {

using clock_type = std::chrono::system_clock;
using duration_type = std::chrono::microseconds;
using time_point_type = std::chrono::time_point<clock_type, duration_type>;

using tempo_type = MilkPowder::Mapping::Message::tempo_type;

using consumer_type = std::function<void(MilkPowder::EventMutableWrapper)>;

class TickClock final {
  static constexpr char TAG[] = "Yogurt::TickClock";
 public:
  explicit TickClock(uint16_t division, uint32_t tempo = 0)
  : division_(division),
    tempo_(tempo) {
    if (!BaseOnTimeCode() && tempo == 0) {
      MilkTea_throw(InvalidParam, "ctor -- tempo is zero but division is not based on timecode");
    }
  }
  TickClock(TickClock &&another)
  : TickClock(another.division_, another.tempo_) {}
  uint32_t operator()(duration_type duration) const {
    auto t = std::chrono::duration_cast<tempo_type>(duration);
    if (BaseOnTimeCode()) {
      return ComputeByTimeCode(t);
    }
    return Compute(t);
  }
  bool BaseOnTimeCode() const {
    return (division_ & 0x8000) != 0;
  }
  uint32_t tempo() const {
    return tempo_;
  }
 private:
  uint32_t Compute(tempo_type duration) const {
    uint64_t count = duration.count();
    count *= division_;
    count /= tempo_;
    return count;
  }
  uint32_t ComputeByTimeCode(tempo_type duration) const {
    uint64_t count = duration.count();
    count *= GetAccuracy();
    count /= std::chrono::duration_cast<tempo_type>(std::chrono::seconds(1)).count();
    return static_cast<uint32_t>(count);
  }
  int16_t GetAccuracy() const {
    int16_t result = 1;
    int16_t division = division_;
    result *= division & 0x00ff;
    result *= -(division >> 010);
    return result;
  }
  const uint16_t division_;
  const uint32_t tempo_;
  MilkTea_NonCopy(TickClock)
  MilkTea_NonMoveAssign(TickClock)
};

} // namespace Yogurt

#endif // ifndef YOGURT_CLOCK_H_
