#ifndef LIB_MILK_POWDER_MESSAGE_H_
#define LIB_MILK_POWDER_MESSAGE_H_

#ifdef __cplusplus
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct MilkPowder_Message_t;

struct MilkPowder_Event_t;
struct MilkPowder_Meta_t;
struct MilkPowder_Sysex_t;

extern
TEA_API uint8_t (TEA_CALL *
MilkPowder_Message_GetType)(const struct MilkPowder_Message_t *);

extern
TEA_API uint32_t (TEA_CALL *
MilkPowder_Message_GetDelta)(const struct MilkPowder_Message_t *);

extern
TEA_API void (TEA_CALL *
MilkPowder_Message_SetDelta)(struct MilkPowder_Message_t *, uint32_t delta);

extern
TEA_API struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Message_FromEvent)(struct MilkPowder_Event_t *);

extern
TEA_API const struct MilkPowder_Event_t * (TEA_CALL *
MilkPowder_Message_ToEvent)(const struct MilkPowder_Message_t *);

extern
TEA_API struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Message_FromMeta)(struct MilkPowder_Meta_t *);

extern
TEA_API const struct MilkPowder_Meta_t * (TEA_CALL *
MilkPowder_Message_ToMeta)(const struct MilkPowder_Message_t *);

extern
TEA_API struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Message_FromSysex)(struct MilkPowder_Sysex_t *);

extern
TEA_API const struct MilkPowder_Sysex_t * (TEA_CALL *
MilkPowder_Message_ToSysex)(const struct MilkPowder_Message_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace MilkPowder {

using Message = MilkPowder_Message_t;

using Event = MilkPowder_Event_t;
using Meta = MilkPowder_Meta_t;
using Sysex = MilkPowder_Sysex_t;

} // namespace MilkPowder

struct MilkPowder_Message_t : tea::mask_type<MilkPowder::Message> {
  auto type() const -> uint8_t {
    return MilkPowder_Message_GetType(get());
  }
  auto delta() const -> uint32_t {
    return MilkPowder_Message_GetDelta(get());
  }
  auto delta(uint32_t d) -> void {
    return MilkPowder_Message_SetDelta(get(), d);
  }
  static
  auto from_event(MilkPowder::Event *&&it) -> MilkPowder::Message * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return MilkPowder_Message_FromEvent(it);
  }
  auto to_event() const-> const MilkPowder::Event * {
    return MilkPowder_Message_ToEvent(get());
  }
  auto is_event() const -> bool {
    auto t = type();
    return t >= 0x80 && t < 0xf0;
  }
  static
  auto from_meta(MilkPowder::Meta *&&it) -> MilkPowder::Message * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return MilkPowder_Message_FromMeta(it);
  }
  auto to_meta() const-> const MilkPowder::Meta * {
    return MilkPowder_Message_ToMeta(get());
  }
  auto is_meta() const -> bool {
    auto t = type();
    return t == 0xff;
  }
  static
  auto from_sysex(MilkPowder::Sysex *&&it) -> MilkPowder::Message * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return MilkPowder_Message_FromSysex(it);
  }
  auto to_sysex() const-> const MilkPowder::Sysex * {
    return MilkPowder_Message_ToSysex(get());
  }
  auto is_sysex() const -> bool {
    auto t = type();
    return t == 0xf0;
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_MESSAGE_H_
