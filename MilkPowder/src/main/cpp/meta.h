#ifndef MILKPOWDER_META_H_
#define MILKPOWDER_META_H_

#include "message.h"

namespace MilkPowder {

class MetaImpl final : public MessageImpl {
  static constexpr char TAG[] = "MilkPowder::MetaImpl";
 public:
  static std::unique_ptr<MetaImpl> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const final;
  std::unique_ptr<MessageImpl> Clone() const final {
    return std::make_unique<MetaImpl>(*this);
  }
  MetaImpl(uint32_t delta, uint8_t type, std::vector<uint8_t> args) : MessageImpl(delta, 0xff), type_(type), args_(std::move(args)) {
    if (type >= 0x80) {
      MilkTea_throwf(InvalidParam, "ctor -- type: %02" PRIx8, type);
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
