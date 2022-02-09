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
    const raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_message_to, another.get(), &obj);
    return obj;
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
  static MutableWrapper<mapping> Parse(std::function<size_t(uint8_t [], size_t)> reader, uint8_t last = 0xff) {
    MilkPowder::Mapping::ByteReader reader_ = reader;
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &obj, reader_, last);
    return obj;
  }
  static MutableWrapper<mapping> Make(uint32_t delta, uint8_t type, std::array<uint8_t, 2> args) {
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_create, &obj, delta, type, args[0], args[1]);
    return obj;
  }
  static MutableWrapper<mapping> From(MutableWrapper<Mapping::Message> &&another) {
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_from_message, &obj, another.get());
    another.release();
    return obj;
  }
};

using EventMutableWrapper = MutableWrapper<Mapping::Event>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_EVENT_H_
