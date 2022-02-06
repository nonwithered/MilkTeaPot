#ifndef YOGURT_CORE_RECORD_H_
#define YOGURT_CORE_RECORD_H_

#include <vector>
#include <list>
#include <chrono>

#include <milkpowder.h>

namespace Yogurt {

using clock_type = std::chrono::system_clock;
using duration_type = std::chrono::microseconds;
using time_point_type = std::chrono::time_point<clock_type, duration_type>;

using tempo_type = MilkPowder::Mapping::Message::tempo_type;

class TickClock final {
 public:
  explicit TickClock(uint16_t division, uint32_t tempo = 60'000'000 / 120)
  : division_(division),
    tempo_(tempo) {}
  TickClock(TickClock &&another)
  : division_(another.division_),
    tempo_(another.tempo_) {}
  uint32_t operator()(duration_type duration) const {
    auto t = std::chrono::duration_cast<tempo_type>(duration);
    if (BaseOnTimeCode()) {
      return ComputeByTimeCode(t);
    }
    return Compute(t);
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
  bool BaseOnTimeCode() const {
    return (division_ & 0x8000) != 0;
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

class TrackTape final {
 public:
  TrackTape(TickClock tick_clock, time_point_type time_point)
  : tick_clock_(std::move(tick_clock)),
    time_point_(time_point) {}
  TrackTape(TrackTape &&another)
  : tick_clock_(std::move(another.tick_clock_)),
    time_point_(another.time_point_),
    items_(std::move(another.items_)) {}
  void Append(MilkPowder::MessageMutableWrapper item, time_point_type time_point = time_point_type()) {
    duration_type duration = time_point - time_point_;
    time_point_ = time_point;
    if (duration < duration_type::zero()) {
      duration = duration_type::zero();
    }
    item.SetDelta(tick_clock_(duration));
    items_.push_back(std::move(item));
  }
 private:
  TickClock tick_clock_;
  time_point_type time_point_;
  std::list<MilkPowder::MessageMutableWrapper> items_;
  MilkTea_NonCopy(TrackTape)
  MilkTea_NonMoveAssign(TrackTape)
};

class RecorderImpl final {
  static constexpr char TAG[] = "Yogurt::RecorderImpl";
 public:
  explicit RecorderImpl(uint16_t division)
  : division_(division) {}
  void Resume() {

  }
  void Pause() {

  }
 private:
  const uint16_t division_;
  std::vector<TrackTape> tapes_;
  MilkTea_NonCopy(RecorderImpl)
  MilkTea_NonMove(RecorderImpl)
};

} // namespace Yogurt

#endif // ifndef YOGURT_CORE_RECORD_H_
