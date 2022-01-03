#ifndef SOYMILK_SORTER_H_
#define SOYMILK_SORTER_H_

#include <list>

#include <soymilk/common.h>

#include "frame.h"

namespace SoyMilk {

namespace Codec {

class TickClock final {
  using duration_type = TeaPot::TimerUnit::duration_type;
  using tempo_type = std::chrono::microseconds;
 public:
  explicit TickClock(uint16_t division)
  : division_(division),
    tempo_(60'000'000 / 120) {
    if (division > 0x7f)  {
      MilkTea_throwf(Unsupported, "try to decode midi but division is %" PRIx16, division);
    }
  }
  void SetTempo(uint32_t tempo) {
    tempo_ = tempo;
  }
  duration_type operator()(uint32_t tick) {
    int64_t tempo = static_cast<int64_t>(static_cast<double>(tick) * tempo_ / division_);
    return std::chrono::duration_cast<duration_type>(tempo_type(tempo));
  }
 private:
  const uint16_t division_;
  uint32_t tempo_;
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
        MilkTea_throwf(Unsupported, "try to decode midi but format is %" PRIx16, format);
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
      case 0x01: {
        std::vector<MilkPowder::TrackConstWrapper> tracks;
        for (uint16_t i = 0; i != ntrks; ++i) {
          tracks.push_back(midi.GetTrack(i));
        }
        TickClock tick_clock(division);
        Collect(tick_clock, ntrks, std::move(tracks));
        break;
      }
      case 0x02:
        for (uint16_t i = 0; i != ntrks; ++i) {
          Collect(division, i, midi.GetTrack(i));
        }
        break;
    }
  }
 private:
  void Collect(TickClock &tick_clock, uint16_t ntrks, std::vector<MilkPowder::TrackConstWrapper> tracks) {
    std::vector<uint32_t> count(ntrks, 0);
    std::vector<uint32_t> index(ntrks, 0);
    std::vector<duration_type> times(ntrks, duration_type::zero());
    std::vector<FrameEventImpl> item;
    for (uint16_t i = 0; i != ntrks; ++i) {
      count[i] = tracks[i].GetCount();
      item.emplace_back(i);
    }
    duration_type time = duration_type::zero();
    MilkTea_loop {
      for (uint16_t i = 0; i != ntrks; ++i) {
        // todo
      }
    }
  }
  void Collect(uint16_t division, uint16_t index, MilkPowder::TrackConstWrapper track) {
    TickClock tick_clock(division);
    auto time = duration_type::zero();
    FrameEventImpl *item = nullptr;
    auto iterator = queue_.begin();
    for (uint32_t i = 0, n = track.GetCount(); i != n; ++i) {
      auto message = track.GetMessage(i);
      auto delta = message.GetDelta();
      if (item == nullptr || delta != 0) {
        time = tick_clock(delta) + time;
        auto tail = queue_.end();
        MilkTea_loop {
          if (iterator == tail || iterator->time() > time) {
            iterator = queue_.insert(iterator, FrameBufferImpl(time));
            break;
          }
          if (iterator->time() == time) {
            break;
          }
          ++iterator;
        }
        item = &iterator->Append(FrameEventImpl(index));
      }
      item->Append(message);
      if (message.IsMeta()) {
        auto meta = MilkPowder::MetaConstWrapper::From(message);
        if (meta.GetType() == 0x51) {
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
          tick_clock.SetTempo(tempo);
        }
      }
    }
  }
  explicit FrameBufferSorterImpl(std::list<FrameBufferImpl> &queue) : queue_(queue) {}
  std::list<FrameBufferImpl> &queue_;
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_SORTER_H_
