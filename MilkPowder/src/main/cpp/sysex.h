#ifndef MILKPOWDER_SYSEX_H_
#define MILKPOWDER_SYSEX_H_

#include <tuple>

#include "except.h"
#include "message.h"

namespace MilkPowder {

class Sysex final : public Message {
 public:
  static std::unique_ptr<Sysex> Parse(const uint8_t *&begin, const uint8_t *const end);
  void Dump(std::vector<uint8_t> &) const final;
  Sysex(std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> items) : Message(std::get<0>(items.front()), 0xf0), items_(std::move(items)) {
    if (items_.size() == 0) {
      throw Except(Except::Type::InvalidParam);
    }
  }
  const std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> &items() const { return items_; }
 private:
  const std::vector<std::tuple<uint32_t, std::vector<uint8_t>>> items_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_SYSEX_H_
