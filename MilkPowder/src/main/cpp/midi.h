#ifndef MILKPOWDER_MIDI_H_
#define MILKPOWDER_MIDI_H_

#include <vector>

#include "except.h"
#include "track.h"

namespace MilkPowder {

class Midi final {
 public:
  static std::unique_ptr<Midi> Parse(const uint8_t *&begin, const uint8_t *const end);
  void Dump(std::vector<uint8_t> &) const;
  Midi(uint16_t format, uint16_t division, std::vector<std::unique_ptr<Track>> items) : format_(format), division_(division), items_(std::move(items)) {}
  uint16_t format() const { return format_; }
  uint16_t division() const { return division_; }
  const std::vector<std::unique_ptr<Track>> &items() const { return items_; };
 private:
  const uint16_t format_;
  const uint16_t division_;
  const std::vector<std::unique_ptr<Track>> items_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MIDI_H_
