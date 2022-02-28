#ifndef LIB_MILK_POWDER_TRACK_H_
#define LIB_MILK_POWDER_TRACK_H_

#ifdef __cplusplus
#include <tea/mask.h>
#include <tea/slice.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#include <milkpowder/message.h>
#include <milkpowder/event.h>
#include <milkpowder/meta.h>
#include <milkpowder/sysex.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct MilkPowder_Track_t;

extern
TEA_API struct MilkPowder_Track_t * (TEA_CALL *
MilkPowder_Track_Create)(struct MilkPowder_Message_t *[], uint32_t count);

extern
TEA_API void (TEA_CALL *
MilkPowder_Track_Destroy)(struct MilkPowder_Track_t *);

extern
TEA_API uint32_t (TEA_CALL *
MilkPowder_Track_GetCount)(const struct MilkPowder_Track_t *);

extern
TEA_API struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Track_GetMessage)(struct MilkPowder_Track_t *, uint32_t index);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace MilkPowder {

using Track = MilkPowder_Track_t;

} // namespace MilkPowder

inline
auto MilkPowder::Message::clone() const -> MilkPowder::Message * {
  if (is_event()) {
    return MilkPowder::Message::from_event(to_event()->clone());
  }
  if (is_meta()) {
    return MilkPowder::Message::from_meta(to_meta()->clone());
  }
  if (is_sysex()) {
    return MilkPowder::Message::from_sysex(to_sysex()->clone());
  }
  return nullptr;
}

struct MilkPowder_Track_t : tea::mask_type<MilkPowder::Track> {
  static
  auto init(tea::slice<MilkPowder::Message *> msg) -> MilkPowder::Track * {
    return MilkPowder_Track_Create(msg.data(), msg.size());
  }
  auto drop() && -> void {
    MilkPowder_Track_Destroy(get());
  }
  auto size() const -> uint32_t {
    return MilkPowder_Track_GetCount(get());
  }
  auto item(uint32_t index) -> MilkPowder::Message * {
    return MilkPowder_Track_GetMessage(get(), index);
  }
  auto item(uint32_t index) const -> const MilkPowder::Message * {
    return MilkPowder_Track_GetMessage(const_cast<MilkPowder::Track *>(get()), index);
  }
  auto operator[](uint32_t index) -> MilkPowder::Message * {
    return item(index);
  }
  auto operator[](uint32_t index) const -> const MilkPowder::Message * {
    return item(index);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_TRACK_H_
