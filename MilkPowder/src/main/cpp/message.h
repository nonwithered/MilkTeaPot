#ifndef MILKPOWDER_MESSAGE_H_
#define MILKPOWDER_MESSAGE_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <milkpowder/common.h>

namespace MilkPowder {

class MessageImpl {
  static constexpr char TAG[] = "MilkPowder::MessageImpl";
 public:
  using mapping = Mapping::Message;
  static MessageImpl &Parse(std::function<std::tuple<uint8_t, bool>()> callback, uint8_t last);
  virtual std::vector<uint8_t> Dump() const = 0;
  virtual MessageImpl &Clone() const = 0;
  virtual ~MessageImpl() = default;
  MessageImpl(uint32_t delta, uint8_t type) : delta_(delta), type_(type) {
    CheckDelta(delta);
    if (type < 0x80 || (type > 0xf0 && type < 0xff)) {
      MilkTea_throwf(InvalidParam, "ctor -- type: %02" PRIx8, type);
    }
  }
  bool IsEvent() const {
    return type_ >= 0x80 && type_ < 0xf0;
  }
  bool IsMeta() const {
    return type_ == 0xff;
  }
  bool IsSysex() const {
    return type_ == 0xf0;
  }
  void delta(uint32_t delta) {
    CheckDelta(delta);
    delta_ = delta;
  }
  uint32_t delta() const { return delta_; }
  uint8_t type() const { return type_; }
 private:
  uint32_t delta_;
  const uint8_t type_;
  static void CheckDelta(uint32_t delta) {
    if (delta > 0x0fff'ffff) {
      MilkTea_throwf(InvalidParam, "delta: %08" PRIx32, delta);
    }
  }
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_MESSAGE_H_
