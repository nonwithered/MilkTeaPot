#ifndef MILKPOWDER_MIDI_H_
#define MILKPOWDER_MIDI_H_

#include <vector>

#include "track.h"

namespace MilkPowder {

class MidiImpl final {
 public:
  static std::unique_ptr<MidiImpl> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const;
  MidiImpl(uint16_t format, uint16_t division, std::vector<std::unique_ptr<TrackImpl>> items) : format_(format), division_(division), items_(std::move(items)) {
    MilkTea_LogPrint(INFO, TAG, "ctor format=%" PRIu16 ", division=%" PRIu16 ", ntrks=%" PRIu16, format_, division_, static_cast<uint16_t>(items_.size()));
    if (format > static_cast<uint16_t>(0x0002)) {
      MilkTea_LogPrint(WARN, TAG, "ctor format %" PRIu16, format);
    }
  }
  MidiImpl(const MidiImpl &another) : format_(another.format_), division_(another.division_), items_(Clone(another.items_)) {
    MilkTea_LogPrint(INFO, TAG, "copy format=%" PRIu16 ", division=%" PRIu16 ", ntrks=%" PRIu16, format_, division_, static_cast<uint16_t>(items_.size()));
  }
  uint16_t format() const { return format_; }
  uint16_t division() const { return division_; }
  const std::vector<std::unique_ptr<TrackImpl>> &items() const { return items_; };
 private:
  const uint16_t format_;
  const uint16_t division_;
  const std::vector<std::unique_ptr<TrackImpl>> items_;
  static std::vector<std::unique_ptr<TrackImpl>> Clone(const std::vector<std::unique_ptr<TrackImpl>> &another) {
    std::vector<std::unique_ptr<TrackImpl>> items;
    for (const auto &it : another) {
      items.emplace_back(new TrackImpl(*it));
    }
    return std::move(items);
  }
  static constexpr char TAG[] = "midi";
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MIDI_H_
