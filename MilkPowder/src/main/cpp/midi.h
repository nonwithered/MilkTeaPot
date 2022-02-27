#ifndef MILKPOWDER_MIDI_H_
#define MILKPOWDER_MIDI_H_

#include <vector>
#include <memory>

#include "track.h"

namespace MilkPowder {
namespace internal {

struct Midi;

} // namespace internal
} // namespace MilkPowder

template<>
struct tea::meta::cast_pair<MilkPowder::internal::Midi> {
  using type = MilkPowder::Midi;
};

namespace MilkPowder {

namespace internal {

using namespace tea;
using namespace tea::meta;
using namespace MilkTea;

struct Midi {
  using item_type = std::unique_ptr<Track>;
  static
  auto Parse(file::input) -> MilkPowder::Midi *;
  auto Dump(file::output) const -> void;
  static
  auto init(uint16_t format, uint16_t division, uint16_t count, MilkPowder::Track *items[]) -> Midi * {
    if (format > 0x0002) {
      err::raise<err_enum::invalid_param>(ToString::FromFormat(
      "create midi but the format is invalid %" PRIx16, format));
      return nullptr;
    }
    if (division == 0) {
      err::raise<err_enum::invalid_param>("create midi but the division is zero");
      return nullptr;
    }
    if (count == 0) {
      err::raise<err_enum::invalid_param>("create midi but the count is zero");
      return nullptr;
    }
    if (items == nullptr) {
      err::raise<err_enum::null_obj>("create midi but the track is nullptr");
      return nullptr;
    }
    std::vector<item_type> vec(count);
    for (uint16_t i = 0; i != count; ++i) {
      vec[i].reset(&wrap_cast<Track>(items[i]));
    }
    return new Midi(format, division, std::move(vec));
  }
  Midi(uint16_t format, uint16_t division, std::vector<item_type> items)
  : format_(format),
    division_(division),
    items_(std::move(items)) {}
  auto size() const -> uint16_t {
    return items_.size();
  }
  auto operator[](uint16_t index) -> MilkPowder::Track * {
    if (auto count = size(); index >= count) {
      err::raise<err_enum::out_of_range>(ToString::FromFormat(
      "midi get a track with index %" PRIu16 " but there are only %" PRIu16 " ones", index, count));
      return nullptr;
    }
    return unwrap_cast(*items_[index]);
  }
  uint16_t format_;
  uint16_t division_;
  std::vector<item_type> items_;
};

} // namespace internal
} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MIDI_H_
