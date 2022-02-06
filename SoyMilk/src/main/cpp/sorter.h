#ifndef SOYMILK_SORTER_H_
#define SOYMILK_SORTER_H_

#include <list>

#include <soymilk/common.h>

#include "frame.h"

namespace SoyMilk {

namespace Codec {

class TickClock final {
  static constexpr char TAG[] = "SoyMilk::Codec::TickClock";
  using duration_type = TeaPot::TimerUnit::duration_type;
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
  void SetTempo(uint32_t tempo, uint32_t tick_current) {
    if (BaseOnTimeCode()) {
      return;
    }
    tempo_map_.emplace_back(tick_current, tempo);
  }
  duration_type operator()(uint32_t tick_delta, uint32_t tick_last) const {
    if (BaseOnTimeCode()) {
      return ComputeByTimeCode(tick_delta);
    }
    auto cursor_last = Find(tick_last);
    auto cursor_next = cursor_last + 1;
    auto tick_target = tick_last + tick_delta;
    auto tick_next = GetTick(*cursor_next);
    if (cursor_next == tempo_map_.end() || tick_next >= tick_target) {
      return Compute(tick_delta, GetTempo(*cursor_last));
    }
    uint32_t tick_until = tick_next - tick_last;
    tick_delta -= tick_until;
    duration_type time = Compute(tick_until, GetTempo(*cursor_last));
    auto cursor_end = Find(tick_target);
    for (auto i = cursor_next, n = cursor_end; i != n; ++i) {
      tick_until = GetTick(*(i + 1)) - GetTick(*i);
      tick_delta -= tick_until;
      time += Compute(tick_until, GetTempo(*i));
    }
    time += Compute(tick_delta, GetTempo(*cursor_end));
    return time;
  }
 private:
  duration_type Compute(uint32_t tick, uint32_t tempo) const {
    uint64_t count = tick;
    count *= tempo;
    count /= division_;
    return std::chrono::duration_cast<duration_type>(tempo_type(static_cast<int64_t>(count)));
  }
  std::vector<std::tuple<uint32_t, uint32_t>>::const_iterator Find(uint32_t tick) const {
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
  duration_type ComputeByTimeCode(uint32_t tick) const {
    auto duration = std::chrono::duration_cast<tempo_type>(std::chrono::seconds(1));
    duration *= tick;
    duration /= GetAccuracy();
    return std::chrono::duration_cast<duration_type>(duration);
  }
  int16_t GetAccuracy() const {
    int16_t result = 1;
    int16_t division = division_;
    result *= division & 0x00ff;
    result *= -(division >> 010);
    return result;
  }
  static uint32_t GetTick(const std::tuple<uint32_t, uint32_t> &tuple) {
    return std::get<0>(tuple);
  }
  static uint32_t GetTempo(const std::tuple<uint32_t, uint32_t> &tuple) {
    return std::get<1>(tuple);
  }
  const uint16_t division_;
  std::vector<std::tuple<uint32_t, uint32_t>> tempo_map_;
};

class FrameBufferSorterImpl final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferSortorImpl";
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  static std::list<FrameBufferImpl> Sort(MilkPowder::MidiConstWrapper midi) {
    std::list<FrameBufferImpl> queue;
    FrameBufferSorterImpl sorter(queue);
    sorter(midi);
    return queue;
  }
  void operator()(MilkPowder::MidiConstWrapper midi) {
    auto format = midi.GetFormat();
    if (format > 0x02) {
        MilkTea_throwf(Unsupported, "try to decode midi but format is %04" PRIx16, format);
    }
    auto ntrks = midi.GetNtrks();
    if (ntrks == 0) {
      MilkTea_logW("ntrks is 0");
      return;
    }
    auto division = midi.GetDivision();
    switch (format) {
      case 0x00:
        Collect(division, 0, midi.GetTrack(0));
        break;
      case 0x01:
      {
        std::vector<MilkPowder::TrackConstWrapper> tracks;
        for (uint16_t i = 0; i != ntrks; ++i) {
          tracks.push_back(midi.GetTrack(i));
        }
        TickClock tick_clock(division);
        Collect(tick_clock, ntrks, std::move(tracks));
      }
        break;
      case 0x02:
        for (uint16_t i = 0; i != ntrks; ++i) {
          Collect(division, i, midi.GetTrack(i));
        }
        break;
    }
  }
 private:
  void Collect(TickClock &tick_clock, uint16_t ntrks, std::vector<MilkPowder::TrackConstWrapper> tracks) {
    std::vector<uint32_t> counts(ntrks, 0);
    std::vector<uint32_t> indices(ntrks, 0);
    std::vector<uint32_t> ticks(ntrks, 0);
    std::vector<duration_type> times(ntrks, duration_type::zero());
    for (uint16_t i = 0; i != ntrks; ++i) {
      counts[i] = tracks[i].GetCount();
    }
    MilkTea_loop {
      duration_type time_current = duration_type(-1);
      for (uint16_t i = 0; i != ntrks; ++i) {
        auto index = indices[i];
        if (index >= counts[i]) {
          continue;
        }
        auto track = tracks[i];
        auto message = track.GetMessage(index);
        auto tick_delta = message.GetDelta();
        auto time_last = times[i];
        auto tick_last = ticks[i];
        duration_type time_msg = time_last + tick_clock(tick_delta, tick_last);
        if (time_current < duration_type::zero() || time_msg < time_current) {
          time_current = time_msg;
        }
      }
      if (time_current < duration_type::zero()) {
        break;
      }
      FrameBufferImpl fbo(time_current);
      for (uint16_t i = 0; i != ntrks; ++i) {
        auto index = indices[i];
        auto count = counts[i];
        if (index >= count) {
          continue;
        }
        auto track = tracks[i];
        auto message = track.GetMessage(index);
        auto tick_delta = message.GetDelta();
        {
          auto time_last = times[i];
          auto tick_last = ticks[i];
          duration_type time_msg = time_last + tick_clock(tick_delta, tick_last);
          if (time_current != time_msg) {
            continue;
          }
        }
        times[i] = time_current;
        uint32_t tick_current = ticks[i] + tick_delta;
        ticks[i] = tick_current;
        FrameEventImpl item(i);
        item.Append(message);
        SetTempoIfNeed(tick_clock, tick_current, message);
        while (++index < count) {
          message = track.GetMessage(index);
          auto delta = message.GetDelta();
          if (delta != 0) {
            break;
          }
          item.Append(message);
          SetTempoIfNeed(tick_clock, tick_current, message);
        }
        indices[i] = index;
        fbo.Append(std::move(item));
      }
      queue_.push_back(std::move(fbo));
    }
  }
  void Collect(uint16_t division, uint16_t index, MilkPowder::TrackConstWrapper track) {
    TickClock tick_clock(division);
    auto time_current = duration_type::zero();
    uint32_t tick_current = 0;
    FrameEventImpl *item = nullptr;
    auto iterator = queue_.begin();
    for (uint32_t i = 0, n = track.GetCount(); i != n; ++i) {
      auto message = track.GetMessage(i);
      auto tick_delta = message.GetDelta();
      if (item == nullptr || tick_delta != 0) {
        time_current = time_current + tick_clock(tick_delta, tick_current);
        tick_current += tick_delta;
        auto tail = queue_.end();
        MilkTea_loop {
          if (iterator == tail || iterator->time() > time_current) {
            iterator = queue_.insert(iterator, FrameBufferImpl(time_current));
            break;
          }
          if (iterator->time() == time_current) {
            break;
          }
          ++iterator;
        }
        item = &iterator->Append(FrameEventImpl(index));
      }
      item->Append(message);
      SetTempoIfNeed(tick_clock, tick_current, message);
    }
  }
  static void SetTempoIfNeed(TickClock &tick_clock, uint32_t tick_current, MilkPowder::MessageConstWrapper message) {
    if (!message.IsMeta()) {
      return;
    }
    auto meta = MilkPowder::MetaConstWrapper::From(message);
    if (meta.GetType() != 0x51) {
      return;
    }
    const uint8_t *data = nullptr;
    uint32_t length = meta.GetArgs(&data);
    if (length != 0x03) {
      auto args = MilkTea::ToStringHex::FromBytes(data, length);
      MilkTea_throwf(InvalidParam, "try to set tempo but argc is %" PRIu32 " and argv is %s", length, args.data());
    }
    uint32_t tempo = 0;
    for (uint32_t i = 0; i != length; ++i) {
      tempo <<= 010;
      tempo |= data[i];
    }
    tick_clock.SetTempo(tempo, tick_current);
  }
  explicit FrameBufferSorterImpl(std::list<FrameBufferImpl> &queue) : queue_(queue) {}
  std::list<FrameBufferImpl> &queue_;
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_SORTER_H_
