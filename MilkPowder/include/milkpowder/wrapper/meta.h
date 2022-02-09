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
    const raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_message_to, another.get(), &obj);
    return obj;
  }
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_invoke_panic(mapping::raw_get_type, get(), &res);
    return res;
  }
  uint32_t GetArgs(const uint8_t **args) const {
    uint32_t res = 0;
    const uint8_t *ptr = nullptr;
    MilkTea_invoke_panic(mapping::raw_get_args, get(), &ptr, &res);
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
  static MutableWrapper<mapping> Parse(std::function<size_t(uint8_t [], size_t)> reader) {
    MilkPowder::Mapping::ByteReader reader_ = reader;
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_parse, &obj, reader_);
    return obj;
  }
  static MutableWrapper<mapping> Make(uint32_t delta, uint8_t type, std::vector<uint8_t> args) {
    uint32_t argc = static_cast<uint32_t>(args.size());
    uint8_t *argv = args.data();
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_create, &obj, delta, type, argv, argc);
    return obj;
  }
  static MutableWrapper<mapping> From(MutableWrapper<Mapping::Message> &&another) {
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(mapping::raw_from_message, &obj, another.get());
    another.release();
    return obj;
  }
};

using MetaMutableWrapper = MutableWrapper<Mapping::Meta>;

} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_WRAPPER_META_H_
