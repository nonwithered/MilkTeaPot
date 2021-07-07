#ifndef MILKPOWDER_MIDI_H_
#define MILKPOWDER_MIDI_H_

#include <vector>

#include "log.h"
#include "except.h"
#include "track.h"

namespace MilkPowder {

class Midi final {
 public:
  static std::unique_ptr<Midi> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const;
  Midi(uint16_t format, uint16_t division, std::vector<std::unique_ptr<Track>> items) : format_(format), division_(division), items_(std::move(items)) {
    LOG_PRINT(INFO, TAG, "ctor format=%" PRIu16 ", division=%" PRIu16 ", ntrks=%" PRIu16, format_, division_, static_cast<uint16_t>(items_.size()));
    if (format > static_cast<uint16_t>(0x0002)) {
      LOG_PRINT(WARN, TAG, "ctor format %" PRIu16, format);
    }
  }
  Midi(const Midi &another) : format_(another.format_), division_(another.division_), items_(Clone(another.items_)) {
    LOG_PRINT(INFO, TAG, "copy format=%" PRIu16 ", division=%" PRIu16 ", ntrks=%" PRIu16, format_, division_, static_cast<uint16_t>(items_.size()));
  }
  uint16_t format() const { return format_; }
  uint16_t division() const { return division_; }
  const std::vector<std::unique_ptr<Track>> &items() const { return items_; };
 private:
  const uint16_t format_;
  const uint16_t division_;
  const std::vector<std::unique_ptr<Track>> items_;
  static std::vector<std::unique_ptr<Track>> Clone(const std::vector<std::unique_ptr<Track>> &another) {
    std::vector<std::unique_ptr<Track>> items;
    for (const auto &it : another) {
      items.emplace_back(new Track(*it));
    }
    return std::move(items);
  }
  static constexpr char TAG[] = "midi";
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MIDI_H_
