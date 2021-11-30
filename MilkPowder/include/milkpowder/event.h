#ifndef LIB_MILKPOWDER_EVENT_H_
#define LIB_MILKPOWDER_EVENT_H_

#ifdef __cplusplus

#include <milkpowder/holder.h>
#include <functional>
#include <tuple>
#include <array>

namespace MilkPowder {

template<>
class ConstInterface<TypeSet::Event> {
  using RawType = typename RawTypeMap<TypeSet::Event>::target;
 public:;
  virtual const RawType *get() const = 0;
 public:
  uint8_t GetType() const {
    uint8_t result = 0;
    MilkTea_panic(MilkPowder_Event_GetType(get(), &result));
    return result;
  }
  std::array<uint8_t, 2> GetArgs() const {
    std::array<uint8_t, 2> result{};
    MilkTea_panic(MilkPowder_Event_GetArgs(get(), result.data()));
    return result;
  }
};

using EventConstWrapper = ConstWrapper<TypeSet::Event>;

template<>
class MutableInterface<TypeSet::Event> {
  using RawType = typename RawTypeMap<TypeSet::Event>::target;
 public:;
  virtual RawType *get() = 0;
 public:
  static MutableWrapper<TypeSet::Event> Parse(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) {
    RawType *self = nullptr;
    MilkTea_panic(RawParseMap<TypeSet::Event>::target(&self, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke, last));
    return self;
  }
  static MutableWrapper<TypeSet::Event> Make(uint32_t delta, uint8_t type, std::array<uint8_t, 2> args) {
    RawType *self = nullptr;
    MilkTea_panic(RawCreateMap<TypeSet::Event>::target(&self, delta, type, args[0], args[1]));
    return self;
  }
};

using EventMutableWrapper = MutableWrapper<TypeSet::Event>;

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_EVENT_H_
