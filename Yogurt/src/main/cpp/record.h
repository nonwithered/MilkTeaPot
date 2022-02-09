#ifndef YOGURT_RECORD_H_
#define YOGURT_RECORD_H_

#include <memory>

#include <yogurt/common.h>

#include "tape.h"

namespace Yogurt {

class RecorderImpl final : public std::enable_shared_from_this<RecorderImpl> {
  static constexpr char TAG[] = "Yogurt::RecorderImpl";
  using State = RecorderState;
 public:
  static std::shared_ptr<RecorderImpl> Make(uint16_t division) {
    return std::make_shared<RecorderImpl>(division);
  }
  explicit RecorderImpl(uint16_t division)
  : division_(division),
    state_(State::PAUSE),
    time_point_(Now()) {
      if (division == 0) {
        MilkTea_throw(InvalidParam, "ctor -- can not be zero");
      }
    }
  auto state() const {
    return state_;
  }
  auto division() const {
    return division_;
  }
  void Resume() {
    if (auto state = state_; state != State::PAUSE) {
      MilkTea_throwf(LogicError, "Resume but state is %s", StateName(state));
    }
    state_ = State::RESUME;
    auto time_point = Now();
    auto duration = time_point - time_point_;
    time_point_ = time_point;
    for (auto &it : tapes_) {
      it.Walk(duration);
    }
  }
  void Pause() {
    if (auto state = state_; state != State::RESUME) {
      MilkTea_throwf(LogicError, "Pause but state is %s", StateName(state));
    }
    state_ = State::PAUSE;
    time_point_ = Now();
  }
  consumer_type Append(TickClock tick_clock) {
    if (auto state = state_; state != State::PAUSE) {
      MilkTea_throwf(LogicError, "Append but state is %s", StateName(state));
    }
    size_t n = tapes_.size();
    tapes_.emplace_back(std::move(tick_clock), time_point_);
    return [weak = weak_from_this(), n](auto it) {
      auto obj = weak.lock();
      if (!obj) {
        MilkTea_throwf(LogicError, "append event but recorder is dead");
      }
      auto state = obj->state();
      if (state == State::CLOSED) {
        MilkTea_throwf(LogicError, "append event but recorder is closed");
      }
      auto &tape = obj->tapes_[n];
      switch (state) {
        case State::RESUME:
          tape.Append(std::move(it), Now());
          return;
        case State::PAUSE:
          tape.Append(std::move(it));
          return;
        default:
          MilkTea_assertf("append event but recorder is %s", StateName(state));
      }
    };
  }
  MilkPowder::MidiMutableWrapper Take() {
    if (auto state = state_; state != State::PAUSE) {
      MilkTea_throwf(LogicError, "Take but state is %s", StateName(state));
    }
    state_ = State::CLOSED;
    std::vector<MilkPowder::TrackMutableWrapper> vec;
    for (auto &it : tapes_) {
      uint32_t tempo = it.GetTempo();
      uint32_t n = it.items().size();
      if (tempo != 0) {
        ++n;
      }
      std::vector<MilkPowder::MessageMutableWrapper> msg(n + 1);
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
      msg[i] = MilkPowder::MutableInterface<MilkPowder::Mapping::Message>::From(MilkPowder::MetaMutableWrapper::Make(0, 0x2f, {}));
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
  const uint16_t division_;
  State state_;
  time_point_type time_point_;
  std::vector<TrackTape> tapes_;
  MilkTea_NonCopy(RecorderImpl)
  MilkTea_NonMove(RecorderImpl)
};

} // namespace Yogurt

#endif // ifndef YOGURT_RECORD_H_
