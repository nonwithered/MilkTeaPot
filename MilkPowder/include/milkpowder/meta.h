#ifndef LIB_MILK_POWDER_META_H_
#define LIB_MILK_POWDER_META_H_

#ifdef __cplusplus
#include <tea/slice.h>
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#include <milkpowder/message.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct MilkPowder_Meta_t;

extern
TEA_API struct MilkPowder_Meta_t * (TEA_CALL *
MilkPowder_Meta_Create)(uint32_t delta, uint8_t type, const uint8_t args[], uint32_t len);

extern
TEA_API void (TEA_CALL *
MilkPowder_Meta_Destroy)(struct MilkPowder_Meta_t *);

extern
TEA_API uint8_t (TEA_CALL *
MilkPowder_Meta_GetType)(const struct MilkPowder_Meta_t *);

extern
TEA_API uint32_t (TEA_CALL *
MilkPowder_Meta_GetArgs)(const struct MilkPowder_Meta_t *, const uint8_t **);

extern
TEA_API struct MilkPowder_Meta_t * (TEA_CALL *
MilkPowder_Meta_FromMessage)(struct MilkPowder_Message_t *);

extern
TEA_API const struct MilkPowder_Message_t * (TEA_CALL *
MilkPowder_Meta_ToMessage)(const struct MilkPowder_Meta_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace MilkPowder {

using Meta = MilkPowder_Meta_t;

} // namespace MilkPowder

struct MilkPowder_Meta_t : tea::mask_type<MilkPowder::Meta> {
  static
  auto init(uint32_t delta, uint8_t type, tea::slice<const uint8_t> args) -> MilkPowder::Meta * {
    return MilkPowder_Meta_Create(delta, type, args.data(), args.size());
  }
  auto drop() && -> void {
    MilkPowder_Meta_Destroy(get());
  }
  auto type() const -> uint8_t {
    return MilkPowder_Meta_GetType(get());
  }
  auto args() const -> tea::slice<const uint8_t> {
    const uint8_t *ptr = nullptr;
    auto len = MilkPowder_Meta_GetArgs(get(), &ptr);
    return tea::slice<const uint8_t>(ptr, len);
  }
  static
  auto from_message(MilkPowder::Message *&&it) -> MilkPowder::Meta * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return MilkPowder_Meta_FromMessage(it);
  }
  auto to_message() const-> const MilkPowder::Message * {
    return MilkPowder_Meta_ToMessage(get());
  }
  auto delta() const -> uint32_t {
    return to_message()->delta();
  }
  auto delta(uint32_t d) -> void {
    auto *message = MilkPowder::Message::from_meta(get());
    if (message == nullptr) {
      return;
    }
    message->delta(d);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_META_H_
