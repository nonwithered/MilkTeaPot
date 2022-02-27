#ifndef MILKPOWDER_TRACK_H_
#define MILKPOWDER_TRACK_H_

#include <vector>

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Track;

} // namespace internal
} // namespace MilkPowder

template<>
struct tea::meta::cast_pair<MilkPowder::internal::Track> {
  using type = MilkPowder::Track;
};

namespace MilkPowder {

namespace internal {

using namespace tea;
using namespace tea::meta;
using namespace MilkTea;

struct Track {
  using item_type = std::unique_ptr<Message>;
  static
  auto init(MilkPowder::Message *items[], uint32_t count) -> Track * {
    if (count == 0) {
      err::raise<err_enum::invalid_param>("create track but the count is zero");
      return nullptr;
    }
    if (items == nullptr) {
      err::raise<err_enum::null_obj>("create track but the message is nullptr");
      return nullptr;
    }
    std::vector<item_type> vec(count);
    for (uint32_t i = 0; i != count; ++i) {
      vec[i].reset(&wrap_cast<Message>(items[i]));
    }
    return new Track(std::move(vec));
  }
  Track(std::vector<item_type> items)
  : items_(std::move(items)) {}
  auto size() const -> uint32_t {
    return items_.size();
  }
  auto operator[](uint32_t index) -> MilkPowder::Message * {
    if (auto count = size(); index >= count) {
      err::raise<err_enum::out_of_range>(ToString::FromFormat(
      "midi get a message with index %" PRIu32 " but there are only %" PRIu32 " ones", index, count));
      return nullptr;
    }
    return unwrap_cast(*items_[index]);
  }
  const std::vector<item_type> items_;
};

} // namespace internal
} // namespace MilkPowder

#endif // ifndef MILKPOWDER_TRACK_H_
