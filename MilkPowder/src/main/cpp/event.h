#ifndef MILKPOWDER_EVENT_H_
#define MILKPOWDER_EVENT_H_

#include <array>

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Event;

} // namespace internal
} // namespace MilkPowder

template<>
struct tea::meta::cast_pair<MilkPowder::internal::Event> {
  using type = MilkPowder::Event;
};

namespace MilkPowder {

namespace internal {

using namespace tea;
using namespace tea::meta;
using namespace MilkTea;

struct Event : Message {
  const std::array<uint8_t, 2> args_;
  auto FromEvent() -> MilkPowder::Message * {
    return unwrap_cast(static_cast<Message &>(*this));
  }
  static
  auto ToEvent(const MilkPowder::Message *it) -> const MilkPowder::Event * {
    return unwrap_cast(reinterpret_cast<const Event &>(wrap_cast<const Message>(it)));
  }
  static
  auto FromMessage(MilkPowder::Message *it) -> MilkPowder::Event * {
    return unwrap_cast(reinterpret_cast<Event &>(wrap_cast<Message>(it)));
  }
  auto ToMessage() const -> const MilkPowder::Message * {
    return unwrap_cast(static_cast<const Message &>(*this));
  }
  static
  auto init(uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1) -> Event * {
    if (type < 0x80 || type >= 0xf0) {
      err::raise<err_enum::invalid_param>(ToString::FromFormat(
      "create event but the type is invalid: %" PRIx8, type));
      return nullptr;
    }
    if (type & 0xf0 == 0xc0 || type & 0xf0 == 0xd0) {
      arg1 = 0;
    }
    return new Event(delta, type, { arg0, arg1 });
  }
  Event(uint32_t delta, uint8_t type, std::array<uint8_t, 2> args)
  : Message(delta, type),
    args_(args) {}
  uint8_t args(uint8_t *p) const {
    if (p != nullptr) {
      *p = args_[1];
    }
    return args_[0];
  }
};

} // namespace internal
} // namespace MilkPowder

#endif // ifndef MILKPOWDER_EVENT_H_
