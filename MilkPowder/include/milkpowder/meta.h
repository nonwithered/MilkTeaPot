#ifndef LIB_MILKPOWDER_META_H_
#define LIB_MILKPOWDER_META_H_

#ifdef __cplusplus

#include <milkpowder/holder.h>
#include <functional>

namespace MilkPowder {

class MetaConstInterface : virtual public TypeConstInterface<TypeSet::Meta> {
 public:
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(MilkPowder_Meta_GetType(Self(), &res));
    return res;
  }
  uint32_t GetArgs(const uint8_t **args) const {
    uint32_t res = 0;
    const uint8_t *ptr = nullptr;
    MilkTea_panic(MilkPowder_Meta_GetArgs(Self(), &ptr, &res));
    if (args != nullptr) {
      *args = ptr;
    }
    return res;
  }
};

class MetaConstWrapper final : virtual public MetaConstInterface {
  friend class MessageConstInterface;
  TypeConstWrapper(Meta, MetaConstWrapper)
};

class MetaInterface : virtual public TypeInterface<TypeSet::Meta> {
 public:
  RawType *release() && {
    return Move(std::forward<TypeInterface<TypeSet::Meta>>(*this));
  }
};

class MetaWrapper final : virtual public MetaConstInterface, virtual public MetaInterface {
  TypeWrapper(Meta, MetaWrapper)
 public:
  explicit MetaWrapper(std::function<bool(uint8_t *)> callback) : MetaWrapper() {
    MilkTea_panic(MilkPowder_Meta_Parse(addr(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
  MetaWrapper(uint32_t delta, uint8_t type, const uint8_t args[], uint32_t length) : MetaWrapper() {
    MilkTea_panic(MilkPowder_Meta_Create(addr(), delta, type, args, length));
  }
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_META_H_
