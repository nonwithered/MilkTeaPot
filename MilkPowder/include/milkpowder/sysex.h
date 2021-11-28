#ifndef LIB_MILKPOWDER_SYSEX_H_
#define LIB_MILKPOWDER_SYSEX_H_

#ifdef __cplusplus

#include <milkpowder/holder.h>
#include <functional>

namespace MilkPowder {

class SysexConstInterface : virtual public TypeConstInterface<TypeSet::Sysex> {
 public:
  void GetArgs(std::function<void(uint32_t, const uint8_t *, uint32_t)> callback) const {
    MilkTea_panic(MilkPowder_Sysex_GetArgs(Self(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
};

class SysexConstWrapper final : virtual public SysexConstInterface {
  friend class MessageConstInterface;
  TypeConstWrapper(Sysex, SysexConstWrapper)
};

class SysexInterface : virtual public TypeInterface<TypeSet::Sysex> {
 public:
  RawType *release() && {
    return Move(std::forward<TypeInterface<TypeSet::Sysex>>(*this));
  }
};

class SysexWrapper final : virtual public SysexConstInterface, virtual public SysexInterface {
  TypeWrapper(Sysex, SysexWrapper)
 public:
  explicit SysexWrapper(std::function<bool(uint8_t *)> callback) : SysexWrapper() {
    MilkTea_panic(MilkPowder_Sysex_Parse(addr(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
  }
  SysexWrapper(uint32_t delta[], const uint8_t *const args[], uint32_t const length[], uint32_t size) : SysexWrapper() {
    MilkTea_panic(MilkPowder_Sysex_Create(addr(), delta, args, length, size));
  }
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_SYSEX_H_
