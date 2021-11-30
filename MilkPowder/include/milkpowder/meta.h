#ifndef LIB_MILKPOWDER_META_H_
#define LIB_MILKPOWDER_META_H_

#ifdef __cplusplus

#include <milkpowder/holder.h>
#include <functional>

namespace MilkPowder {

template<>
class ConstInterface<TypeSet::Meta> {
  using RawType = typename RawTypeMap<TypeSet::Meta>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(MilkPowder_Meta_GetType(get(), &res));
    return res;
  }
  uint32_t GetArgs(const uint8_t **args) const {
    uint32_t res = 0;
    const uint8_t *ptr = nullptr;
    MilkTea_panic(MilkPowder_Meta_GetArgs(get(), &ptr, &res));
    if (args != nullptr) {
      *args = ptr;
    }
    return res;
  }
};

using MetaConstWrapper = ConstWrapper<TypeSet::Meta>;

template<>
class MutableInterface<TypeSet::Meta> {
  using RawType = typename RawTypeMap<TypeSet::Meta>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  static MutableWrapper<TypeSet::Meta> Parse(std::function<bool(uint8_t *)> callback) {
    RawType *self = nullptr;
    MilkTea_panic(RawParseMap<TypeSet::Meta>::target(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return self;
  }
  static MutableWrapper<TypeSet::Meta> Make(uint32_t delta, uint8_t type, std::vector<uint8_t> args) {
    uint32_t argc = static_cast<uint32_t>(args.size());
    uint8_t *argv = args.data();
    RawType *self = nullptr;
    MilkTea_panic(RawCreateMap<TypeSet::Meta>::target(&self, delta, type, argv, argc));
    return self;
  }
};

using MetaMutableWrapper = MutableWrapper<TypeSet::Meta>;

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_META_H_
