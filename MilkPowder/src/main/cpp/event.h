#ifndef MILKPOWDER_EVENT_H_
#define MILKPOWDER_EVENT_H_

#include <array>

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Event : Message {
  const std::array<uint8_t, 2> args_;
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
