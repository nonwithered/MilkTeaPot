#ifndef LIB_MILKPOWDER_EVENT_H_
#define LIB_MILKPOWDER_EVENT_H_

#include <functional>

#include <milkpowder/wrapper/message.h>

namespace MilkPowder {

template<>
class ConstInterface<Mapping::Event> {
  using mapping = Mapping::Event;
  using raw_type = mapping::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  static ConstWrapper<mapping> From(ConstWrapper<Mapping::Message> another) {
    const raw_type *self = nullptr;
    MilkTea_invoke_panic(mapping::raw_message_to, another.get(), &self);
    return self;
  }
  uint8_t GetType() const {
    uint8_t result = 0;
    MilkTea_invoke_panic(mapping::raw_get_type, get(), &result);
    return result;
  }
  std::array<uint8_t, 2> GetArgs() const {
    std::array<uint8_t, 2> result{};
    MilkTea_invoke_panic(mapping::raw_get_args, get(), result.data());
    return result;
  }
};

using EventConstWrapper = ConstWrapper<Mapping::Event>;

template<>
class MutableInterface<Mapping::Event> {
  using mapping = Mapping::Event;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  static MutableWrapper<mapping> Parse(std::function<bool(uint8_t *)> reader, uint8_t last = 0xff) {
    raw_type *self = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &self, Mapping::Reader(reader), last);
    return self;
  }
  static MutableWrapper<mapping> Make(uint32_t delta, uint8_t type, std::array<uint8_t, 2> args) {
    raw_type *self = nullptr;
    MilkTea_invoke_panic(mapping::raw_create, &self, delta, type, args[0], args[1]);
    return self;
  }
  static MutableWrapper<mapping> From(MutableWrapper<Mapping::Message> &&another) {
    raw_type *self = nullptr;
    MilkTea_invoke_panic(mapping::raw_from_message, &self, another.get());
    another.release();
    return self;
  }
};

using EventMutableWrapper = MutableWrapper<Mapping::Event>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_EVENT_H_
