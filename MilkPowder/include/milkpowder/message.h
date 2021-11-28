#ifndef LIB_MILKPOWDER_MESSAGE_H_
#define LIB_MILKPOWDER_MESSAGE_H_

#ifdef __cplusplus

#include <milkpowder/event.h>
#include <milkpowder/meta.h>
#include <milkpowder/sysex.h>

namespace MilkPowder {

class MessageConstInterface : virtual public TypeConstInterface<TypeSet::Message> {
 public:
  uint32_t GetDelta() const {
    uint32_t result = 0;
    MilkTea_panic(MilkPowder_Message_GetDelta(Self(), &result));
    return result;
  }
  uint8_t GetType() const {
    uint8_t result = 0;
    MilkTea_panic(MilkPowder_Message_GetType(Self(), &result));
    return result;
  }
  bool IsEvent() const {
    bool result = false;
    MilkTea_panic(MilkPowder_Message_IsEvent(Self(), &result));
    return result;
  }
  bool IsMeta() const {
    bool result = false;
    MilkTea_panic(MilkPowder_Message_IsMeta(Self(), &result));
    return result;
  }
  bool IsSysex() const {
    bool result = false;
    MilkTea_panic(MilkPowder_Message_IsSysex(Self(), &result));
    return result;
  }
  EventConstWrapper ToEvent() const {
    const RawTypeMap<TypeSet::Event>::target *result = nullptr;
    MilkTea_panic(MilkPowder_Message_ToEvent(Self(), &result));
    return EventConstWrapper(result);
  }
  MetaConstWrapper ToMeta() const {
    const RawTypeMap<TypeSet::Meta>::target *result = nullptr;
    MilkTea_panic(MilkPowder_Message_ToMeta(Self(), &result));
    return MetaConstWrapper(result);
  }
  SysexConstWrapper ToSysex() const {
    const RawTypeMap<TypeSet::Sysex>::target *result = nullptr;
    MilkTea_panic(MilkPowder_Message_ToSysex(Self(), &result));
    return SysexConstWrapper(result);
  }
};

class MessageConstWrapper final : virtual public MessageConstInterface {
  friend class TrackConstInterface;
  TypeConstWrapper(Message, MessageConstWrapper)
};

class MessageInterface : virtual public TypeInterface<TypeSet::Message> {
 public:
  void SetDelta(uint32_t delta) {
    MilkTea_panic(MilkPowder_Message_SetDelta(Self(), delta));
  }
  RawType *release() && {
    return Move(std::forward<TypeInterface<TypeSet::Message>>(*this));
  }
};

class MessageWrapper final : virtual public MessageConstInterface, virtual public MessageInterface {
  TypeWrapper(Message, MessageWrapper)
 public:
  explicit MessageWrapper(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) : MessageWrapper() {
    MilkTea_panic(MilkPowder_Message_Parse(addr(), &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke, last));
  }
  explicit MessageWrapper(EventWrapper &&another) : MessageWrapper() {
    MilkTea_panic(MilkPowder_Message_FromEvent(addr(), std::forward<EventWrapper>(another).release()));
  }
  explicit MessageWrapper(MetaWrapper &&another) : MessageWrapper() {
    MilkTea_panic(MilkPowder_Message_FromMeta(addr(), std::forward<MetaWrapper>(another).release()));
  }
  explicit MessageWrapper(SysexWrapper &&another) : MessageWrapper() {
    MilkTea_panic(MilkPowder_Message_FromSysex(addr(), std::forward<SysexWrapper>(another).release()));
  }
};

} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MESSAGE_H_
