#ifndef MILKPOWDER_SYSEX_H_
#define MILKPOWDER_SYSEX_H_

#include <tuple>
#include <string>

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Sysex;

} // namespace internal
} // namespace MilkPowder

template<>
struct tea::meta::cast_pair<MilkPowder::internal::Sysex> {
  using type = MilkPowder::Sysex;
};

namespace MilkPowder {

namespace internal {

using namespace tea;
using namespace tea::meta;
using namespace MilkTea;

struct Sysex : Message {
  using item_type = std::tuple<uint32_t, std::vector<uint8_t>>;
  auto FromSysex() -> MilkPowder::Message * {
    return unwrap_cast(static_cast<Message &>(*this));
  }
  static
  auto ToSysex(const MilkPowder::Message *it) -> const MilkPowder::Sysex * {
    return unwrap_cast(static_cast<const Sysex &>(wrap_cast<const Message>(it)));
  }
  static
  auto FromMessage(MilkPowder::Message *it) -> MilkPowder::Sysex * {
    return unwrap_cast(static_cast<Sysex &>(wrap_cast<Message>(it)));
  }
  auto ToMessage() const -> const MilkPowder::Message * {
    return unwrap_cast(static_cast<const Message &>(*this));
  }
  static
  auto init(const uint32_t delta[], const uint8_t *const args[], const uint32_t len[], uint32_t count) -> Sysex * {
    if (count == 0) {
      err::raise<err_enum::invalid_param>("create sysex but the count is zero");
      return nullptr;
    }
    if (delta == nullptr) {
      err::raise<err_enum::null_obj>("create sysex but the delta is nullptr");
      return nullptr;
    }
    if (args == nullptr) {
      err::raise<err_enum::null_obj>("create sysex but the args is nullptr");
      return nullptr;
    }
    if (len == nullptr) {
      err::raise<err_enum::null_obj>("create sysex but the len is nullptr");
      return nullptr;
    }
    std::vector<item_type> vec(count);
    for (uint32_t i = 0; i != count; ++i) {
      vec[i] = std::make_tuple(delta[i], std::vector<uint8_t>(args[i], args[i] + len[i]));
    }
    return new Sysex(std::move(vec));
  }
  Sysex(std::vector<item_type> items)
  : Message(std::get<0>(items.front()), 0xf0),
    items_(std::move(items)) {}
  auto size() const -> uint32_t {
    return items_.size();
  }
  auto args(uint32_t index, const uint8_t **p) const -> uint32_t {
    if (auto count = size(); index >= count) {
      err::raise<err_enum::out_of_range>(ToString::FromFormat(
      "sysex get args with index %" PRIu32 " but there are only %" PRIu32 " ones", index, count));
      return 0;
    }
    auto &item = std::get<1>(items_[index]);
    if (p != nullptr) {
      *p = item.data();
    }
    return item.size();
  }
  auto delta(uint32_t index) const -> uint32_t {
    if (auto count = size(); index >= count) {
      err::raise<err_enum::out_of_range>(ToString::FromFormat(
      "sysex get delta with index %" PRIu32 " but there are only %" PRIu32 " ones", index, count));
      return 0;
    }
    return std::get<0>(items_[index]);
  }
  std::vector<item_type> items_;
  auto Clone() const -> MilkPowder::Sysex * {
    return unwrap_cast(*new Sysex(*this));
  }
};

} // namespace internal
} // namespace MilkPowder

#endif // ifndef MILKPOWDER_SYSEX_H_
