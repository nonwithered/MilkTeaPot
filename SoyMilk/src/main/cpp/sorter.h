#ifndef SOYMILK_SORTER_H_
#define SOYMILK_SORTER_H_

#include <vector>

#include "frame.h"
#include "clock.h"

namespace SoyMilk {

class FrameBufferSorterImpl final {
  static constexpr char TAG[] = "SoyMilk::FrameBufferSortorImpl";
  using tempo_type = MilkPowder::Mapping::Message::tempo_type;
  using key_type = tempo_type;
  using items_type = std::map<key_type, FramePackageImpl>;
 public:
  using queue_type = std::vector<FrameBufferImpl>;
  static queue_type Generate(std::vector<MilkPowder::MidiConstWrapper> vec) {
    FrameBufferSorterImpl sorter;
    for (size_t i = 0, n = vec.size(); i != n; ++i) {
      sorter.Add(vec[i], i);
    }
    return std::move(sorter).Generate();
  }
  queue_type Generate() && {
    queue_type queue(items_.size());
    size_t idx = 0;
    auto &items = items_;
    std::for_each(items.begin(), items.end(), [&queue, &idx](auto &it) {
      queue[idx++] = FrameBufferImpl(it.first, std::move(it.second));
    });
    return queue;
  }
  void Add(MilkPowder::MidiConstWrapper midi, size_t index) {
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
      case 0x00: {
        TickClock tick_clock(division);
        Collect(tick_clock, midi.GetTrack(0), index, 0);
      }
        break;
      case 0x01: {
        std::vector<MilkPowder::TrackConstWrapper> tracks(ntrks);
        for (uint16_t i = 0; i != ntrks; ++i) {
          tracks[i].reset(midi.GetTrack(i).get());
        }
        TickClock tick_clock(division);
        Collect(tick_clock, std::move(tracks), index);
      }
        break;
      case 0x02:
        for (uint16_t ntrk = 0; ntrk != ntrks; ++ntrk) {
          TickClock tick_clock(division);
          Collect(tick_clock, midi.GetTrack(ntrk), index, ntrk);
        }
        break;
    }
  }
 private:
  void Collect(TickClock &tick_clock, std::vector<MilkPowder::TrackConstWrapper> tracks, size_t seq) {
    uint16_t ntrks = tracks.size();
    const std::vector<uint32_t> counts = [ntrks, &tracks]() -> auto {
      std::vector<uint32_t> vec(ntrks);
      for (uint16_t i = 0; i != ntrks; ++i) {
        vec[i] = tracks[i].GetCount();
      }
      return vec;
    }();
    std::vector<uint32_t> indices(ntrks, 0);
    std::vector<uint64_t> ticks(ntrks, 0);
    std::vector<tempo_type> times(ntrks, tempo_type::zero());
    MilkTea_loop {
      tempo_type time_current = tempo_type(-1);
      for (uint16_t i = 0; i != ntrks; ++i) {
        auto index = indices[i];
        if (index >= counts[i]) {
          continue;
        }
        auto track = tracks[i];
        auto message = track.GetMessage(index);
        auto tick_delta = message.GetDelta();
        auto tick_last = ticks[i];
        auto tick_current = tick_last + tick_delta;
        auto time_last = times[i];
        tempo_type time_msg = time_last + tick_clock(tick_last, tick_current);
        if (time_current < tempo_type::zero() || time_msg < time_current) {
          time_current = time_msg;
        }
      }
      if (time_current < tempo_type::zero()) {
        break;
      }
      auto &item_package = Obtain(time_current);
      auto &item_midi = item_package.Obtain(seq);
      for (uint16_t i = 0; i != ntrks; ++i) {
        auto index = indices[i];
        auto count = counts[i];
        if (index >= count) {
          continue;
        }
        auto track = tracks[i];
        auto message = track.GetMessage(index);
        auto tick_delta = message.GetDelta();
        auto tick_last = ticks[i];
        auto tick_current = tick_last + tick_delta;
        {
          auto time_last = times[i];
          tempo_type time_msg = time_last + tick_clock(tick_last, tick_current);
          if (time_current != time_msg) {
            continue;
          }
        }
        auto &item_track = item_midi.Obtain(i);
        times[i] = time_current;
        ticks[i] = tick_current;
        item_track.Append(message);
        SetTempoIfNeed(tick_clock, tick_current, message);
        while (++index < count) {
          message = track.GetMessage(index);
          auto delta = message.GetDelta();
          if (delta != 0) {
            break;
          }
          item_track.Append(message);
          SetTempoIfNeed(tick_clock, tick_current, message);
        }
        indices[i] = index;
      }
    }
  }
  void Collect(TickClock &tick_clock, MilkPowder::TrackConstWrapper track, size_t seq, uint16_t ntrk) {
    auto time_current = tempo_type::zero();
    uint64_t tick_current = 0;
    for (uint32_t i = 0, n = track.GetCount(); i != n; ++i) {
       auto message = track.GetMessage(i);
       auto tick_last = tick_current;
       auto tick_delta = message.GetDelta();
       if (tick_delta != 0) {
         tick_current += tick_delta;
         time_current += tick_clock(tick_last, tick_current);
       }
       auto &item_package = Obtain(time_current);
       auto &item_midi = item_package.Obtain(seq);
       auto &item_track = item_midi.Obtain(ntrk);
       item_track.Append(message);
      SetTempoIfNeed(tick_clock, tick_current, message);
    }
  }
  static void SetTempoIfNeed(TickClock &tick_clock, uint64_t tick_current, MilkPowder::MessageConstWrapper message) {
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
  FramePackageImpl &Obtain(key_type time) {
    return items_.try_emplace(time).first->second;
  }
  items_type items_;
  FrameBufferSorterImpl() = default;
  MilkTea_NonCopy(FrameBufferSorterImpl)
  MilkTea_NonMove(FrameBufferSorterImpl)
};

} // namespace SoyMilk

#endif // ifndef SOYMILK_SORTER_H_
