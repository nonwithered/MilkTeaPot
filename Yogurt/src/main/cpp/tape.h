#ifndef YOGURT_TAPE_H_
#define YOGURT_TAPE_H_

#include <vector>
#include <list>

#include "clock.h"

namespace Yogurt {

class TrackTape final {
  static constexpr char TAG[] = "Yogurt::TrackTape";
 public:
  TrackTape(TickClock tick_clock, time_point_type time_point, std::list<MilkPowder::EventMutableWrapper> items = {})
  : tick_clock_(std::move(tick_clock)),
    time_point_(time_point),
    items_(std::move(items)) {}
  TrackTape(TrackTape &&another)
  : TrackTape(std::move(another.tick_clock_), another.time_point_, std::move(another.items_)) {}
  void Append(MilkPowder::EventMutableWrapper event, time_point_type time_point = time_point_type()) {
    tempo_type duration = time_point - time_point_;
    time_point_ = time_point;
    if (duration < tempo_type::zero()) {
      duration = tempo_type::zero();
    }
    auto message = MilkPowder::MutableInterface<MilkPowder::Mapping::Message>::From(std::move(event));
    message.SetDelta(tick_clock_(duration));
    items_.emplace_back(MilkPowder::MutableInterface<MilkPowder::Mapping::Event>::From(std::move(message)));
  }
  void Walk(tempo_type duration) {
    time_point_ += duration;
  }
  uint32_t GetTempo() const {
    if (tick_clock_.BaseOnTimeCode()) {
      return 0;
    }
    return tick_clock_.tempo();
  }
  auto &items() {
    return items_;
  }
 private:
  TickClock tick_clock_;
  time_point_type time_point_;
  std::list<MilkPowder::EventMutableWrapper> items_;
  MilkTea_NonCopy(TrackTape)
  MilkTea_NonMoveAssign(TrackTape)
};

} // namespace Yogurt

#endif // ifndef YOGURT_TAPE_H_
