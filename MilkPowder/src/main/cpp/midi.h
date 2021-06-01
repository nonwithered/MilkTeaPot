#ifndef MILKPOWDER_MIDI_H_
#define MILKPOWDER_MIDI_H_

#include <vector>

#include "except.h"
#include "track.h"

namespace MilkPowder {

class Midi final {
 public:
  static std::unique_ptr<Midi> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const;
  Midi(uint16_t format, uint16_t division, std::vector<std::unique_ptr<Track>> items) : format_(format), division_(division), items_(std::move(items)) {}
  Midi(const Midi &another) : format_(another.format_), division_(another.division_), items_(Clone(another.items_)) {}
  uint16_t format() const { return format_; }
  uint16_t division() const { return division_; }
  const std::vector<std::unique_ptr<Track>> &items() const { return items_; };
 private:
  const uint16_t format_;
  const uint16_t division_;
  const std::vector<std::unique_ptr<Track>> items_;
  static std::vector<std::unique_ptr<Track>> Clone(const std::vector<std::unique_ptr<Track>> &another) {
    std::vector<std::unique_ptr<Track>> items(another.size());
    for (const auto &it : another) {
      items.emplace_back(new Track(*it));
    }
    return std::move(items);
  }
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MIDI_H_
