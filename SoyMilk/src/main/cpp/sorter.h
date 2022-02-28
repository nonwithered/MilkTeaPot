#ifndef SOYMILK_SORTER_H_
#define SOYMILK_SORTER_H_

#include <vector>

#include <tea.h>
#include <milktea.h>

#include "frame.h"
#include "clock.h"

namespace SoyMilk {

namespace internal {

class FrameBufferSorterImpl final : tea::with_log<FrameBufferSorterImpl>, tea::remove_move {
  using key_type = duration_type;
  using items_type = std::map<key_type, FramePackageImpl>;
 public:
  static constexpr char TAG[] = "FrameBufferSortor";
  using queue_type = std::vector<FrameBufferImpl>;
  static queue_type Generate(std::vector<const MilkPowder::Midi *> vec) {
    FrameBufferSorterImpl sorter;
    for (size_t i = 0, n = vec.size(); i != n; ++i) {
      sorter.Add(vec[i], i);
      if (auto e = tea::err::init(); e != nullptr) {
        tea::err::raise<tea::err_enum::undefined_format>("sorter generate", e);
      }
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
  void Add(const MilkPowder::Midi *midi, size_t index) {
    auto format = midi->format();
    if (format > 0x02) {
      tea::err::raise<tea::err_enum::undefined_format>(MilkTea::ToString::FromFormat(
      "try to decode midi but format is %04" PRIx16, format));
      return;
    }
    auto ntrks = midi->size();
    if (ntrks == 0) {
      log<W>("ntrks is zero");
      return;
    }
    auto division = midi->division();
    switch (format) {
      case 0x00: {
        TickClock tick_clock(division);
        Collect(tick_clock, midi->item(0), index, 0);
      }
        break;
      case 0x01: {
        std::vector<const MilkPowder::Track *> tracks(ntrks);
        for (uint16_t i = 0; i != ntrks; ++i) {
          tracks[i] = midi->item(i);
        }
        TickClock tick_clock(division);
        Collect(tick_clock, std::move(tracks), index);
      }
        break;
      case 0x02:
        for (uint16_t ntrk = 0; ntrk != ntrks; ++ntrk) {
          TickClock tick_clock(division);
          Collect(tick_clock, midi->item(ntrk), index, ntrk);
        }
        break;
    }
  }
 private:
  void Collect(TickClock &tick_clock, std::vector<const MilkPowder::Track *> tracks, size_t seq) {
    uint16_t ntrks = tracks.size();
    const std::vector<uint32_t> counts = [ntrks, &tracks]() -> auto {
      std::vector<uint32_t> vec(ntrks);
      for (uint16_t i = 0; i != ntrks; ++i) {
        vec[i] = tracks[i]->size();
      }
      return vec;
    }();
    std::vector<uint32_t> indices(ntrks, 0);
    std::vector<uint64_t> ticks(ntrks, 0);
    std::vector<duration_type> times(ntrks, duration_type::zero());
    while (true) {
      duration_type time_current = duration_type(-1);
      for (uint16_t i = 0; i != ntrks; ++i) {
        auto index = indices[i];
        if (index >= counts[i]) {
          continue;
        }
        auto track = tracks[i];
        auto message = track->item(index);
        auto tick_delta = message->delta();
        auto tick_last = ticks[i];
        auto tick_current = tick_last + tick_delta;
        auto time_last = times[i];
        duration_type time_msg = time_last + tick_clock(tick_last, tick_current);
        if (time_current < duration_type::zero() || time_msg < time_current) {
          time_current = time_msg;
        }
      }
      if (time_current < duration_type::zero()) {
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
        auto message = track->item(index);
        auto tick_delta = message->delta();
        auto tick_last = ticks[i];
        auto tick_current = tick_last + tick_delta;
        {
          auto time_last = times[i];
          duration_type time_msg = time_last + tick_clock(tick_last, tick_current);
          if (time_current != time_msg) {
            continue;
          }
        }
        auto &item_track = item_midi.Obtain(i);
        times[i] = time_current;
        ticks[i] = tick_current;
        item_track.Append(message->clone());
        SetTempoIfNeed(tick_clock, tick_current, message);
        while (++index < count) {
          message = track->item(index);
          auto delta = message->delta();
          if (delta != 0) {
            break;
          }
          item_track.Append(message->clone());
          SetTempoIfNeed(tick_clock, tick_current, message);
        }
        indices[i] = index;
      }
    }
  }
  void Collect(TickClock &tick_clock, const MilkPowder::Track *track, size_t seq, uint16_t ntrk) {
    auto time_current = duration_type::zero();
    uint64_t tick_current = 0;
    for (uint32_t i = 0, n = track->size(); i != n; ++i) {
       auto message = track->item(i);
       auto tick_last = tick_current;
       auto tick_delta = message->delta();
       if (tick_delta != 0) {
         tick_current += tick_delta;
         time_current += tick_clock(tick_last, tick_current);
       }
       auto &item_package = Obtain(time_current);
       auto &item_midi = item_package.Obtain(seq);
       auto &item_track = item_midi.Obtain(ntrk);
       item_track.Append(message->clone());
      SetTempoIfNeed(tick_clock, tick_current, message);
    }
  }
  static void SetTempoIfNeed(TickClock &tick_clock, uint64_t tick_current, const MilkPowder::Message *message) {
    if (!message->is_meta()) {
      return;
    }
    auto meta = message->to_meta();
    if (meta->type() != 0x51) {
      return;
    }
    auto data = meta->args();
    if (uint32_t length = data.size(); length != 0x03) {
      auto args = MilkTea::ToStringHex::FromBytes(data.data(), length);
      tea::err::raise<tea::err_enum::invalid_param>(MilkTea::ToString::FromFormat(
      "try to set tempo but argc is %" PRIu32 " and argv is %s", length, args.data()));
    }
    uint32_t tempo = data[0] << 020 | data[1] << 010 | data[2];
    tick_clock.SetTempo(tempo, tick_current);
  }
  FramePackageImpl &Obtain(key_type time) {
    return items_.try_emplace(time).first->second;
  }
  items_type items_;
  FrameBufferSorterImpl() = default;
};

} // namespace internal
} // namespace SoyMilk

#endif // ifndef SOYMILK_SORTER_H_
