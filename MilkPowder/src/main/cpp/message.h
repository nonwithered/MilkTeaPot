#ifndef MILKPOWDER_MESSAGE_H_
#define MILKPOWDER_MESSAGE_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <tea.h>
#include <milktea.h>

#include <milkpowder.h>

namespace MilkPowder {

namespace internal {

struct Message;

} // namespace internal
} // namespace MilkPowder

template<>
struct tea::meta::cast_pair<MilkPowder::internal::Message> {
  using type = MilkPowder::Message;
};

namespace MilkPowder {

namespace internal {

struct Message {
  uint32_t delta_;
  const uint8_t type_;
  Message(uint32_t delta, uint8_t type)
  : delta_(delta),
    type_(type) {}
};

} // namespace internal
} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MESSAGE_H_
