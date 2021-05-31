#ifndef MILKPOWDER_META_H_
#define MILKPOWDER_META_H_

#include "except.h"
#include "message.h"

namespace MilkPowder {

class Meta final : public Message {
 public:
  static std::unique_ptr<Meta> Parse(const uint8_t *&begin, const uint8_t *const end);
  void Dump(std::vector<uint8_t> &) const final;
  Meta(uint32_t delta, uint8_t type, std::vector<uint8_t> args) : Message(delta, 0xff), type_(type), args_(std::move(args)) {
    if (type >= 0x80) {
      throw Except(Except::Type::InvalidParam);
    }
  }
  uint8_t type() const { return type_; }
  const std::vector<uint8_t> &args() const { return args_; }
 private:
  const uint8_t type_;
  const std::vector<uint8_t> args_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_META_H_
