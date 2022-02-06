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

enum class RecordState {
  PAUSE,
  RESUME,
  CLOSED,
};

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

class TrackTape final {
  static constexpr char TAG[] = "Yogurt::TrackTape";
 public:
  TrackTape(TickClock tick_clock, time_point_type time_point)
  : tick_clock_(std::move(tick_clock)),
    time_point_(time_point) {}
  TrackTape(TrackTape &&another)
  : tick_clock_(std::move(another.tick_clock_)),
    time_point_(another.time_point_),
    items_(std::move(another.items_)) {}
  void Append(MilkPowder::EventMutableWrapper event, time_point_type time_point = time_point_type()) {
    duration_type duration = time_point - time_point_;
    time_point_ = time_point;
    if (duration < duration_type::zero()) {
      duration = duration_type::zero();
    }
    auto message = MilkPowder::MutableInterface<MilkPowder::Mapping::Message>::From(std::move(event));
    message.SetDelta(tick_clock_(duration));
    items_.emplace_back(MilkPowder::MutableInterface<MilkPowder::Mapping::Event>::From(std::move(message)));
  }
  void Walk(duration_type duration) {
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

class RecorderImpl final {
  static constexpr char TAG[] = "Yogurt::RecorderImpl";
 public:
  explicit RecorderImpl(uint16_t division)
  : division_(division),
    state_(RecordState::PAUSE),
    time_point_(Now()) {}
  RecordState state() const {
    return state_;
  }
  void Resume() {
    if (auto state = state_; state != RecordState::PAUSE) {
      MilkTea_throwf(LogicError, "Resume but state is %s", StateName(state));
    }
    state_ = RecordState::RESUME;
    auto time_point = Now();
    auto duration = time_point - time_point_;
    time_point_ = time_point;
    for (auto &it : tapes_) {
      it.Walk(duration);
    }
  }
  void Pause() {
    if (auto state = state_; state != RecordState::RESUME) {
      MilkTea_throwf(LogicError, "Pause but state is %s", StateName(state));
    }
    state_ = RecordState::PAUSE;
    time_point_ = Now();
  }
  MilkPowder::MidiMutableWrapper Take() {
    if (auto state = state_; state != RecordState::PAUSE) {
      MilkTea_throwf(LogicError, "Take but state is %s", StateName(state));
    }
    std::vector<MilkPowder::TrackMutableWrapper> vec;
    for (auto &it : tapes_) {
      uint32_t tempo = it.GetTempo();
      uint32_t n = it.items().size();
      if (tempo != 0) {
        ++n;
      }
      std::vector<MilkPowder::MessageMutableWrapper> msg(n);
      size_t i = 0;
      if (tempo != 0) {
        std::vector<uint8_t> args = {
          static_cast<uint8_t>((tempo >> 020) & 0xff),
          static_cast<uint8_t>((tempo >> 010) & 0xff),
          static_cast<uint8_t>((tempo >> 000) & 0xff),
        };
        msg[i++] = MilkPowder::MutableInterface<MilkPowder::Mapping::Message>::From(MilkPowder::MetaMutableWrapper::Make(0, 51, std::move(args)));
      }
      for (auto &ev : it.items()) {
        msg[i++] = MilkPowder::MutableInterface<MilkPowder::Mapping::Message>::From(std::move(ev));
      }
      vec.emplace_back(MilkPowder::TrackMutableWrapper::Make(msg.begin(), msg.size()));
    }
    uint16_t ntrk = vec.size();
    if (ntrk == 0) {
      return nullptr;
    }
    uint16_t format = ntrk == 1 ? 0 : 2;
    uint16_t division = division_;
    return MilkPowder::MidiMutableWrapper::Make(format, ntrk, division, vec.begin());
  }
 private:
  static time_point_type Now() {
    return std::chrono::time_point_cast<duration_type>(clock_type::now());
  }
  static const char *StateName(RecordState state) {
    switch (state) {
      case RecordState::PAUSE: return "PAUSE";
      case RecordState::RESUME: return "RESUME";
      default:
        MilkTea_assert("StateName");
    }
  }
  const uint16_t division_;
  RecordState state_;
  time_point_type time_point_;
  std::vector<TrackTape> tapes_;
  MilkTea_NonCopy(RecorderImpl)
  MilkTea_NonMove(RecorderImpl)
};

} // namespace Yogurt

#endif // ifndef YOGURT_CORE_RECORD_H_
