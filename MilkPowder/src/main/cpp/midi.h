#ifndef MILKPOWDER_MIDI_H_
#define MILKPOWDER_MIDI_H_

#include <vector>
#include <memory>

#include "track.h"

namespace MilkPowder {

namespace internal {

struct Midi {
  Midi(uint16_t format, uint16_t division, std::vector<std::unique_ptr<Track>> items)
  : format_(format),
    division_(division),
    items_(std::move(items)) {}
  uint16_t format_;
  uint16_t division_;
  std::vector<std::unique_ptr<Track>> items_;
};

} // namespace internal

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MIDI_H_
