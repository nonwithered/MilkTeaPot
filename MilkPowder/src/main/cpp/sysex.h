#ifndef MILKPOWDER_SYSEX_H_
#define MILKPOWDER_SYSEX_H_

#include <tuple>
#include <string>

#include "message.h"

namespace MilkPowder {

class SysexImpl final : public MessageImpl {
  static constexpr char TAG[] = "MilkPowder::SysexImpl";
 public:
  struct Item {
    uint32_t delta_;
    std::vector<uint8_t> args_;
    Item(uint32_t delta, std::vector<uint8_t> args) : delta_(delta), args_(std::move(args)) {}
  };
  using mapping = Mapping::Sysex;
  static bool message_is(const MessageImpl &it) {
    return it.IsSysex();
  }
  static SysexImpl &Parse(std::function<std::tuple<uint8_t, bool>()> callback);
  std::vector<uint8_t> Dump() const final;
  MessageImpl &Clone() const final {
    return *new SysexImpl(*this);
  }
  SysexImpl(std::vector<Item> items) : MessageImpl(items.front().delta_, 0xf0), items_(std::move(items)) {
    if (items_.size() == 0) {
      MilkTea_throw(InvalidParam, "ctor -- size: 0");
    }
    for (size_t i = 0; i < items_.size() - 1; ++i) {
      const auto &vec = items_[i].args_;
      if (vec.size() != 0 && vec.back() == static_cast<uint8_t>(0xf7)) {
        MilkTea_logW("ctor: invalid 0xf7");
      }
    }
    const auto &vec = items_.back().args_;
    if (vec.size() == 0 || vec.back() != static_cast<uint8_t>(0xf7)) {
      MilkTea_logW("ctor: need 0xf7");
    }
  }
  const std::vector<Item> &items() const { return items_; }
  std::vector<Item> &items() { return items_; }
 private:
  std::vector<Item> items_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_SYSEX_H_
