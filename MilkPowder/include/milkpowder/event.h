#ifndef LIB_MILKPOWDER_EVENT_H_
#define LIB_MILKPOWDER_EVENT_H_

#ifdef __cplusplus

#include <milkpowder/holder.h>
#include <functional>
#include <tuple>

namespace MilkPowder {

class EventConstInterface : virtual public TypeConstInterface<TypeSet::Event> {
 public:
  uint8_t GetType() const {
    uint8_t result = 0;
    MilkTea_panic(MilkPowder_Event_GetType(Self(), &result));
    return result;
  }
  std::tuple<uint8_t, uint8_t> GetArgs() const {
    uint8_t result[2] = {0};
    MilkTea_panic(MilkPowder_Event_GetArgs(Self(), result));
    return std::make_tuple(result[0], result[1]);
  }
};

class EventConstWrapper final : virtual public EventConstInterface {
  friend class MessageConstInterface;
  TypeConstWrapper(Event, EventConstWrapper)
};

class EventInterface : virtual public TypeInterface<TypeSet::Event> {
 public:
  RawType *release() && {
    return Move(std::forward<TypeInterface<TypeSet::Event>>(*this));
  }
};

class EventWrapper final : virtual public EventConstInterface, virtual public EventInterface {
  TypeWrapper(Event, EventWrapper)
 public:
  explicit EventWrapper(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) : EventWrapper() {
    MilkTea_panic(MilkPowder_Event_Parse(addr(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke, last));
  }
  EventWrapper(uint32_t delta, uint8_t type, std::tuple<uint8_t, uint8_t> args) : EventWrapper() {
    MilkTea_panic(MilkPowder_Event_Create(addr(), delta, type, std::get<0>(args), std::get<1>(args)));
  }
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_EVENT_H_
