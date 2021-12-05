#ifndef MILKPOWDER_EVENT_H_
#define MILKPOWDER_EVENT_H_

#include <tuple>

#include "message.h"

namespace MilkPowder {

class EventImpl final : public MessageImpl {
  static constexpr char TAG[] = "MilkPowder::EventImpl";
 public:
  using mapping = Mapping::Event;
  static bool message_is(const MessageImpl &it) {
    return it.IsEvent();
  }
  static EventImpl &Parse(std::function<std::tuple<uint8_t, bool>()> callback, uint8_t last);
  std::vector<uint8_t> Dump() const final;
  MessageImpl &Clone() const final {
    return *new EventImpl(*this);
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
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_EVENT_H_
