#ifndef MILKPOWDER_EVENT_H_
#define MILKPOWDER_EVENT_H_

#include <tuple>

#include "message.h"

namespace MilkPowder {

class EventImpl final : public MessageImpl {
 public:
  static std::unique_ptr<EventImpl> Parse(std::function<std::tuple<uint8_t, bool>()> callback, uint8_t last);
  void Dump(std::vector<uint8_t> &) const final;
  std::unique_ptr<MessageImpl> Clone() const final {
    return std::make_unique<EventImpl>(*this);
  }
  EventImpl(uint32_t delta, uint8_t type, std::tuple<uint8_t, uint8_t> args) : MessageImpl(delta, type), args_(args) {
    if (type >= 0xf0) {
      MilkTea_throwf(InvalidParam, "ctor -- type: %02" PRIx8, type);
    }
    if (std::get<0>(args) >= 0x80 || std::get<1>(args) >= 0x80) {
      MilkTea_throwf(InvalidParam, "ctor -- args: [ %02" PRIx8 ", %02" PRIx8 " ]", std::get<0>(args), std::get<1>(args));
    }
  }
  std::tuple<uint8_t, uint8_t> args() const { return args_; }
 private:
  const std::tuple<uint8_t, uint8_t> args_;
  static constexpr char TAG[] = "MilkPowder#Event";
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_EVENT_H_
