#ifndef MILKPOWDER_MESSAGE_H_
#define MILKPOWDER_MESSAGE_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <tea.h>

namespace MilkPowder {

namespace internal {

struct Message {
  uint32_t delta_;
  const uint8_t type_;
  virtual ~Message() = default;
  Message(uint32_t delta, uint8_t type)
  : delta_(delta),
    type_(type) {}
};

} // namespace internal

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MESSAGE_H_
