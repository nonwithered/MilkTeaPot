#ifndef LIB_MILKPOWDER_MESSAGE_H_
#define LIB_MILKPOWDER_MESSAGE_H_

#ifdef __cplusplus
#include <milkpowder/event.h>
#include <milkpowder/meta.h>
#include <milkpowder/sysex.h>
namespace MilkPowder {
class MessageRef final : public MilkPowder_HolderRef(Message) {
 public:
  explicit MessageRef(const MilkPowder_Message_t *ref) : MilkPowder_HolderRef(Message)(ref) {}
  bool IsEvent() const {
    bool res = false;
    MilkTea_panic(MilkPowder_Message_IsEvent(*this, &res));
    return res;
  }
  bool IsMeta() const {
    bool res = false;
    MilkTea_panic(MilkPowder_Message_IsMeta(*this, &res));
    return res;
  }
  bool IsSysex() const {
    bool res = false;
    MilkTea_panic(MilkPowder_Message_IsSysex(*this, &res));
    return res;
  }
  uint32_t GetDelta() const {
    uint32_t res = 0;
    MilkTea_panic(MilkPowder_Message_GetDelta(*this, &res));
    return res;
  }
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(MilkPowder_Message_GetType(*this, &res));
    return res;
  }
  EventRef ToEvent() const {
    const MilkPowder_Event_t *ref = nullptr;
    MilkTea_panic(MilkPowder_Message_ToEvent(*this, &ref));
    return EventRef(ref);
  }
  MetaRef ToMeta() const {
    const MilkPowder_Meta_t *ref = nullptr;
    MilkTea_panic(MilkPowder_Message_ToMeta(*this, &ref));
    return MetaRef(ref);
  }
  SysexRef ToSysex() const {
    const MilkPowder_Sysex_t *ref = nullptr;
    MilkTea_panic(MilkPowder_Message_ToSysex(*this, &ref));
    return SysexRef(ref);
  }
};
class Message final : public MilkPowder_Holder(Message) {
 public:
  Message() {}
  explicit Message(const MessageRef &ref) : MilkPowder_Holder(Message)(ref) {}
  explicit Message(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) {
    MilkTea_panic(MilkPowder_Message_Parse(&*this, &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke, last));
  }
  explicit Message(Event &&another) {
    MilkTea_panic(MilkPowder_Message_FromEvent(&*this, another.release()));
  }
  explicit Message(Meta &&another) {
    MilkTea_panic(MilkPowder_Message_FromMeta(&*this, another.release()));
  }
  explicit Message(Sysex &&another) {
    MilkTea_panic(MilkPowder_Message_FromSysex(&*this, another.release()));
  }
  void SetDelta(uint32_t delta) {
    MilkTea_panic(MilkPowder_Message_SetDelta(*this, delta));
  }
  EventRef ToEvent() const {
    return MessageRef(*this).ToEvent();
  }
  MetaRef ToMeta() const {
    return MessageRef(*this).ToMeta();
  }
  SysexRef ToSysex() const {
    return MessageRef(*this).ToSysex();
  }
  bool IsEvent() const {
    return MessageRef(*this).IsEvent();
  }
  bool IsMeta() const {
    return MessageRef(*this).IsMeta();
  }
  bool IsSysex() const {
    return MessageRef(*this).IsSysex();
  }
  uint32_t GetDelta() const {
    return MessageRef(*this).GetDelta();
  }
  uint8_t GetType() const {
    return MessageRef(*this).GetType();
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MESSAGE_H_
