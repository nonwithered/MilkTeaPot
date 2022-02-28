#ifndef LIB_MILK_POWDER_EVENT_H_
#define LIB_MILK_POWDER_EVENT_H_

#ifdef __cplusplus
#include <array>
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#include <milkpowder/message.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct MilkPowder_Event_t;

extern
TEA_API struct MilkPowder_Event_t * (TEA_CALL *
MilkPowder_Event_Create)(uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1);

extern
TEA_API void (TEA_CALL *
MilkPowder_Event_Destroy)(struct MilkPowder_Event_t *);

extern
TEA_API struct MilkPowder_Event_t * (TEA_CALL *
MilkPowder_Event_Clone)(const struct MilkPowder_Event_t *);

extern
TEA_API uint8_t (TEA_CALL *
MilkPowder_Event_GetType)(const struct MilkPowder_Event_t *);

extern
TEA_API uint8_t (TEA_CALL *
MilkPowder_Event_GetArgs)(const struct MilkPowder_Event_t *, uint8_t *);

extern
TEA_API struct MilkPowder_Event_t * (TEA_CALL *
MilkPowder_Event_FromMessage)(struct MilkPowder_Message_t *);

extern
TEA_API const struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Event_ToMessage)(const struct MilkPowder_Event_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace MilkPowder {

using Event = MilkPowder_Event_t;

} // namespace MilkPowder

struct MilkPowder_Event_t : tea::mask_type<MilkPowder::Event> {
  static
  auto init(uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1) -> MilkPowder::Event * {
    return MilkPowder_Event_Create(delta, type, arg0, arg1);
  }
  auto drop() && -> void {
    MilkPowder_Event_Destroy(get());
  }
  auto clone() const -> MilkPowder::Event * {
    return MilkPowder_Event_Clone(get());
  }
  auto type() const -> uint8_t {
    return MilkPowder_Event_GetType(get());
  }
  auto args() const -> std::array<uint8_t, 2> {
    std::array<uint8_t, 2> arr;
    arr[0] = MilkPowder_Event_GetArgs(get(), &arr[1]);
    return arr;
  }
  static
  auto from_message(MilkPowder::Message *&&it) -> MilkPowder::Event * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return MilkPowder_Event_FromMessage(it);
  }
  auto to_message() const-> const MilkPowder::Message * {
    return MilkPowder_Event_ToMessage(get());
  }
  auto delta() const -> uint32_t {
    return to_message()->delta();
  }
  auto delta(uint32_t d) -> void {
    auto *message = MilkPowder::Message::from_event(get());
    if (message == nullptr) {
      return;
    }
    message->delta(d);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_EVENT_H_
