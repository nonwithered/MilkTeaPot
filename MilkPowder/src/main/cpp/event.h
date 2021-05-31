#ifndef MILKPOWDER_EVENT_H_
#define MILKPOWDER_EVENT_H_

#include <tuple>

#include "except.h"
#include "message.h"

namespace MilkPowder {

class Event final : public Message {
 public:
  static std::unique_ptr<Event> Parse(const uint8_t *&begin, const uint8_t *const end, uint8_t last);
  void Dump(std::vector<uint8_t> &) const final;
  std::unique_ptr<Message> Clone() const final {
    return std::unique_ptr<Message>(new Event(*this));
  }
  Event(uint32_t delta, uint8_t type, std::tuple<uint8_t, uint8_t> args) : Message(delta, type), args_(args) {
    if (type >= 0xf0 || std::get<0>(args) >= 0x80 || std::get<1>(args) >= 0x80) {
      throw Except(Except::Type::InvalidParam);
    }
  }
  std::tuple<uint8_t, uint8_t> args() const { return args_; }
 private:
  const std::tuple<uint8_t, uint8_t> args_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_EVENT_H_
