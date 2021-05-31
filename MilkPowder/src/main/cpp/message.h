#ifndef MILKPOWDER_MESSAGE_H_
#define MILKPOWDER_MESSAGE_H_

#include <cstdint>
#include <vector>
#include <memory>

#include "except.h"

namespace MilkPowder {

class Message {
 public:
  static std::unique_ptr<Message> Parse(const uint8_t *&begin, const uint8_t *const end);
  virtual void Dump(std::vector<uint8_t> &) const = 0;
  virtual ~Message() = default;
  Message(uint32_t delta, uint8_t type) : delta_(delta), type_(type) {
    if (delta > 0x0fff'ffff || type < 0x80 || (type > 0xf0 && type < 0xff)) {
      throw Except(Except::Type::InvalidParam);
    }
  }
  uint32_t delta() const { return delta_; }
  uint8_t type() const { return type_; }
 private:
  const uint32_t delta_;
  const uint8_t type_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MESSAGE_H_