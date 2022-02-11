#ifndef SOYMILK_CLOCK_H_
#define SOYMILK_CLOCK_H_

#include <soymilk/common.h>

namespace SoyMilk {

class TickClock final {
  static constexpr char TAG[] = "SoyMilk::TickClock";
  using tempo_type = MilkPowder::Mapping::Message::tempo_type;
 public:
  explicit TickClock(uint16_t division)
  : division_(division),
    tempo_map_() {
    if (BaseOnTimeCode()) {
      return;
    }
    tempo_map_.emplace_back(0, 60'000'000 / 120);
  }
  void SetTempo(uint32_t tempo, uint64_t tick_current) {
    if (BaseOnTimeCode()) {
      return;
    }
    tempo_map_.emplace_back(tick_current, tempo);
  }
  tempo_type operator()(uint64_t tick_begin, uint64_t tick_end) const {
    uint32_t tick_delta = static_cast<uint32_t>(tick_end - tick_begin);
    if (BaseOnTimeCode()) {
      return ComputeByTimeCode(tick_delta);
    }
    auto cursor_last = Find(tick_begin);
    auto cursor_next = cursor_last + 1;
    auto tick_next = GetTick(*cursor_next);
    if (cursor_next == tempo_map_.end() || tick_next >= tick_end) {
      return Compute(tick_delta, GetTempo(*cursor_last));
    }
    uint32_t tick_until = tick_next - tick_begin;
    tick_delta -= tick_until;
    tempo_type time = Compute(tick_until, GetTempo(*cursor_last));
    auto cursor_end = Find(tick_end);
    for (auto i = cursor_next, n = cursor_end; i != n; ++i) {
      tick_until = GetTick(*(i + 1)) - GetTick(*i);
      tick_delta -= tick_until;
      time += Compute(tick_until, GetTempo(*i));
    }
    time += Compute(tick_delta, GetTempo(*cursor_end));
    return time;
  }
 private:
  tempo_type Compute(uint32_t tick, uint32_t tempo) const {
    uint64_t count = tick;
    count *= tempo;
    count /= division_;
    return tempo_type(static_cast<int64_t>(count));
  }
  std::vector<std::tuple<uint64_t, uint32_t>>::const_iterator Find(uint64_t tick) const {
    for (auto i = tempo_map_.begin(), n = tempo_map_.end(); ; ++i) {
      auto next = i + 1;
      if (next == n || GetTick(*next) > tick) {
        return i;
      }
    }
  }
  bool BaseOnTimeCode() const {
    return (division_ & 0x8000) != 0;
  }
  tempo_type ComputeByTimeCode(uint32_t tick) const {
    auto duration = std::chrono::duration_cast<tempo_type>(std::chrono::seconds(1));
    duration *= tick;
    duration /= GetAccuracy();
    return duration;
  }
  int16_t GetAccuracy() const {
    int16_t result = 1;
    int16_t division = division_;
    result *= division & 0x00ff;
    result *= -(division >> 010);
    return result;
  }
  static uint32_t GetTick(const std::tuple<uint64_t, uint32_t> &tuple) {
    return std::get<0>(tuple);
  }
  static uint32_t GetTempo(const std::tuple<uint64_t, uint32_t> &tuple) {
    return std::get<1>(tuple);
  }
  const uint16_t division_;
  std::vector<std::tuple<uint64_t, uint32_t>> tempo_map_;
};

} // namespace SoyMilk

#endif // ifndef SOYMILK_CLOCK_H_
