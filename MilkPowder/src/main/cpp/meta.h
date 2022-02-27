#ifndef MILKPOWDER_META_H_
#define MILKPOWDER_META_H_

#include "message.h"

namespace MilkPowder {

namespace internal {

struct Meta;

} // namespace internal
} // namespace MilkPowder

template<>
struct tea::meta::cast_pair<MilkPowder::internal::Meta> {
  using type = MilkPowder::Meta;
};

namespace MilkPowder {

namespace internal {

using namespace tea;
using namespace tea::meta;
using namespace MilkTea;

struct Meta : Message {
  const uint8_t type_;
  const std::vector<uint8_t> args_;
  auto FromMeta() -> MilkPowder::Message * {
    return unwrap_cast(static_cast<Message &>(*this));
  }
  static
  auto ToMeta(const MilkPowder::Message *it) -> const MilkPowder::Meta * {
    return unwrap_cast(reinterpret_cast<const Meta &>(wrap_cast<const Message>(it)));
  }
  static
  auto FromMessage(MilkPowder::Message *it) -> MilkPowder::Meta * {
    return unwrap_cast(reinterpret_cast<Meta &>(wrap_cast<Message>(it)));
  }
  auto ToMessage() const -> const MilkPowder::Message * {
    return unwrap_cast(static_cast<const Message &>(*this));
  }
  static
  auto init(uint32_t delta, uint8_t type, const uint8_t args[], uint32_t len) -> Meta * {
    if (type > 0x80) {
      err::raise<err_enum::invalid_param>(ToString::FromFormat(
      "create meta but the type is invalid: %" PRIx8, type));
      return nullptr;
    }
    if (args == nullptr) {
      err::raise<err_enum::null_obj>("create meta but the args is nullptr");
      return nullptr;
    }
    return new Meta(delta, type, std::vector<uint8_t>(args, args + len));
  }
  Meta(uint32_t delta, uint8_t type, std::vector<uint8_t> args)
  : Message(delta, 0xff),
    type_(type),
    args_(std::move(args)) {}
  auto args(const uint8_t **p) -> uint32_t {
    if (p != nullptr) {
      *p = args_.data();
    }
    return args_.size();
  }
};

} // namespace internal
} // namespace MilkPowder

#endif // ifndef MILKPOWDER_META_H_
