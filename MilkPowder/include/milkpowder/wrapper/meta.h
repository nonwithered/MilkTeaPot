#ifndef LIB_MILKPOWDER_WRAPPER_META_H_
#define LIB_MILKPOWDER_WRAPPER_META_H_

#include <functional>

#include <milkpowder/wrapper/message.h>

namespace MilkPowder {

template<>
class ConstInterface<Mapping::Meta> {
  using mapping = Mapping::Meta;
  using raw_type = mapping::raw_type;
 public:;
  virtual const raw_type *get() const = 0;
 public:
  static ConstWrapper<mapping> From(ConstWrapper<Mapping::Message> another) {
    const raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_message_to(another.get(), &self));
    return self;
  }
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(mapping::raw_get_type(get(), &res));
    return res;
  }
  uint32_t GetArgs(const uint8_t **args) const {
    uint32_t res = 0;
    const uint8_t *ptr = nullptr;
    MilkTea_panic(mapping::raw_get_args(get(), &ptr, &res));
    if (args != nullptr) {
      *args = ptr;
    }
    return res;
  }
};

using MetaConstWrapper = ConstWrapper<Mapping::Meta>;

template<>
class MutableInterface<Mapping::Meta> {
  using mapping = Mapping::Meta;
  using raw_type = mapping::raw_type;
 public:;
  virtual raw_type *get() = 0;
 public:
  static MutableWrapper<mapping> Parse(std::function<bool(uint8_t *)> callback) {
    raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_parse(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<mapping> Make(uint32_t delta, uint8_t type, std::vector<uint8_t> args) {
    uint32_t argc = static_cast<uint32_t>(args.size());
    uint8_t *argv = args.data();
    raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_create(&self, delta, type, argv, argc));
    return self;
  }
  static MutableWrapper<mapping> From(MutableWrapper<Mapping::Message> &&another) {
    raw_type *self = nullptr;
    MilkTea_panic(mapping::raw_from_message(&self, another.get()));
    another.release();
    return self;
  }
};

using MetaMutableWrapper = MutableWrapper<Mapping::Meta>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_META_H_
