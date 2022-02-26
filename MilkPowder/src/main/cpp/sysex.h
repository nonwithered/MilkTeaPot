#ifndef MILKPOWDER_SYSEX_H_
#define MILKPOWDER_SYSEX_H_

#include <tuple>
#include <string>

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Sysex : Message {
  using item_type = std::tuple<uint32_t, std::vector<uint8_t>>;
  Sysex(std::vector<item_type> items)
  : Message(std::get<0>(items.front()), 0xf0),
    items_(std::move(items)) {}
  std::vector<item_type> items_;
};

} // namespace internal

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_SYSEX_H_
