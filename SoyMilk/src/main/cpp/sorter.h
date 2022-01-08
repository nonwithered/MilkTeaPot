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
  using tempo_type = std::chrono::microseconds;
 public:
  explicit TickClock(uint16_t division)
  : division_(division),
    tempo_map_() {
    if (division > 0x7fff)  {
      MilkTea_throwf(Unsupported, "try to decode midi but division is %04" PRIx16, division);
    }
    tempo_map_.emplace_back(duration_type::zero(), 60'000'000 / 120);
  }
  void SetTempo(uint32_t tempo, duration_type time) {
    tempo_map_.emplace_back(time, tempo);
  }
  duration_type operator()(uint32_t tick, duration_type last) {
    return Compute(tick, Find(last));
  }
 private:
  duration_type Compute(uint32_t tick, uint32_t tempo) {
    int64_t tempo_ = static_cast<int64_t>(static_cast<double>(tick) * tempo / division_);
    return std::chrono::duration_cast<duration_type>(tempo_type(tempo_));
  }
  uint32_t Find(duration_type time) {
    for (auto i = tempo_map_.begin(), n = tempo_map_.end(); ; ++i) {
      auto next = i + 1;
      if (next == n || std::get<0>(*next) > time) {
        return std::get<1>(*i);
      }
    }
  }
  const uint16_t division_;
  std::vector<std::tuple<duration_type, uint32_t>> tempo_map_;
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
        auto time_last = times[i];
        duration_type time_msg = time_last + tick_clock(message.GetDelta(), time_last);
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
        {
          auto time_last = times[i];
          duration_type time_msg = time_last + tick_clock(message.GetDelta(), time_last);
          if (time_current != time_msg) {
            continue;
          }
        }
        times[i] = time_current;
        FrameEventImpl item(i);
        item.Append(message);
        SetTempoIfNeed(tick_clock, time_current, message);
        while (++index < count) {
          message = track.GetMessage(index);
          auto delta = message.GetDelta();
          if (delta != 0) {
            break;
          }
          item.Append(message);
          SetTempoIfNeed(tick_clock, time_current, message);
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
    FrameEventImpl *item = nullptr;
    auto iterator = queue_.begin();
    for (uint32_t i = 0, n = track.GetCount(); i != n; ++i) {
      auto message = track.GetMessage(i);
      auto delta = message.GetDelta();
      if (item == nullptr || delta != 0) {
        time_current = time_current + tick_clock(delta, time_current);
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
      SetTempoIfNeed(tick_clock, time_current, message);
    }
  }
  static void SetTempoIfNeed(TickClock &tick_clock, duration_type time_current, MilkPowder::MessageConstWrapper message) {
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
      auto args = MilkTea::ToStringHexFromBytes(data, length);
      auto argc = length;
      auto *argv = args.data();
      MilkTea_throwf(InvalidParam, "try to set tempo but argc is %" PRIu32 " and argv is %s", argc, argv);
    }
    uint32_t tempo = 0;
    for (uint32_t i = 0; i != length; ++i) {
      tempo <<= 010;
      tempo |= data[i];
    }
    tick_clock.SetTempo(tempo, time_current);
  }
  explicit FrameBufferSorterImpl(std::list<FrameBufferImpl> &queue) : queue_(queue) {}
  std::list<FrameBufferImpl> &queue_;
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_SORTER_H_
