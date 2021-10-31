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
    MilkTea_panic(MilkPowder_Message_IsEvent(get(), &res));
    return res;
  }
  bool IsMeta() const {
    bool res = false;
    MilkTea_panic(MilkPowder_Message_IsMeta(get(), &res));
    return res;
  }
  bool IsSysex() const {
    bool res = false;
    MilkTea_panic(MilkPowder_Message_IsSysex(get(), &res));
    return res;
  }
  uint32_t GetDelta() const {
    uint32_t res = 0;
    MilkTea_panic(MilkPowder_Message_GetDelta(get(), &res));
    return res;
  }
  uint8_t GetType() const {
    uint8_t res = 0;
    MilkTea_panic(MilkPowder_Message_GetType(get(), &res));
    return res;
  }
  EventRef ToEvent() const {
    const MilkPowder_Event_t *ref = nullptr;
    MilkTea_panic(MilkPowder_Message_ToEvent(get(), &ref));
    return EventRef(ref);
  }
  MetaRef ToMeta() const {
    const MilkPowder_Meta_t *ref = nullptr;
    MilkTea_panic(MilkPowder_Message_ToMeta(get(), &ref));
    return MetaRef(ref);
  }
  SysexRef ToSysex() const {
    const MilkPowder_Sysex_t *ref = nullptr;
    MilkTea_panic(MilkPowder_Message_ToSysex(get(), &ref));
    return SysexRef(ref);
  }
};
class Message final : public MilkPowder_Holder(Message) {
 public:
  explicit Message(MilkPowder_Message_t *ptr = nullptr) : MilkPowder_Holder(Message)(ptr) {}
  explicit Message(const MessageRef &ref) : MilkPowder_Holder(Message)(ref) {}
  explicit Message(std::function<bool(uint8_t *)> callback, uint8_t last = 0xff) : Message() {
    MilkTea_panic(MilkPowder_Message_Parse(reset(), &callback, MilkTea::CallbackToken<decltype(callback)>::Invoke, last));
  }
  explicit Message(Event &&another) : Message() {
    MilkTea_panic(MilkPowder_Message_FromEvent(reset(), another.release()));
  }
  explicit Message(Meta &&another) : Message() {
    MilkTea_panic(MilkPowder_Message_FromMeta(reset(), another.release()));
  }
  explicit Message(Sysex &&another) : Message() {
    MilkTea_panic(MilkPowder_Message_FromSysex(reset(), another.release()));
  }
  void SetDelta(uint32_t delta) {
    MilkTea_panic(MilkPowder_Message_SetDelta(get(), delta));
  }
  EventRef ToEvent() const {
    return MessageRef(get()).ToEvent();
  }
  MetaRef ToMeta() const {
    return MessageRef(get()).ToMeta();
  }
  SysexRef ToSysex() const {
    return MessageRef(get()).ToSysex();
  }
  bool IsEvent() const {
    return MessageRef(get()).IsEvent();
  }
  bool IsMeta() const {
    return MessageRef(get()).IsMeta();
  }
  bool IsSysex() const {
    return MessageRef(get()).IsSysex();
  }
  uint32_t GetDelta() const {
    return MessageRef(get()).GetDelta();
  }
  uint8_t GetType() const {
    return MessageRef(get()).GetType();
  }
};
} // namespace MilkPowder
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKPOWDER_MESSAGE_H_
