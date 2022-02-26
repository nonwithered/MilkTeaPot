#ifndef MILKPOWDER_META_H_
#define MILKPOWDER_META_H_

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Meta : Message {
  const uint8_t type_;
  const std::vector<uint8_t> args_;
  Meta(uint32_t delta, uint8_t type, std::vector<uint8_t> args)
  : Message(delta, 0xff),
    type_(type),
    args_(std::move(args)) {}
  auto args(const uint8_t **p) -> uint32_t {
    if (p != nullptr) {
      *p = args_.data();
    }
    return args_.size();
  }
};

} // namespace internal

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_META_H_
