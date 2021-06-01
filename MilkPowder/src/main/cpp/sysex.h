#ifndef MILKPOWDER_SYSEX_H_
#define MILKPOWDER_SYSEX_H_

#include <tuple>

#include "except.h"
#include "message.h"

namespace MilkPowder {

class Sysex final : public Message {
 public:
  static std::unique_ptr<Sysex> Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  void Dump(std::vector<uint8_t> &) const final;
  std::unique_ptr<Message> Clone() const final {
    return std::unique_ptr<Message>(new Sysex(*this));
  }
  Sysex(std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> items) : Message(std::get<0>(items.front()), 0xf0), items_(std::move(items)) {
    if (items_.size() == 0) {
      LOG_PRINT(ERROR, TAG, "ctor size %" PRIu32, static_cast<uint32_t>(items_.size()));
      throw Except(Except::Type::InvalidParam);
    }
  }
  const std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> &items() const { return items_; }
 private:
  const std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> items_;
  static constexpr char TAG[] = "sysex";
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_SYSEX_H_
