#ifndef LIB_MILK_POWDER_META_H_
#define LIB_MILK_POWDER_META_H_

#ifdef __cplusplus
#include <tea/slice.h>
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct milk_powder_message_t;

struct milk_powder_meta_t;

extern
TEA_API struct milk_powder_meta_t * (TEA_CALL *
milk_powder_meta_create)(uint32_t delta, uint8_t type, uint8_t args[], uint32_t len);

extern
TEA_API void (TEA_CALL *
milk_powder_meta_destroy)(struct milk_powder_meta_t *);

extern
TEA_API uint8_t (TEA_CALL *
milk_powder_meta_type)(const struct milk_powder_meta_t *);

extern
TEA_API uint32_t (TEA_CALL *
milk_powder_meta_args)(const struct milk_powder_meta_t *, const uint8_t **);

extern
TEA_API struct milk_powder_meta_t * (TEA_CALL *
milk_powder_meta_from_message)(struct milk_powder_message_t *);

extern
TEA_API const struct milk_powder_message_t * (TEA_CALL *
milk_powder_meta_to_message)(const struct milk_powder_meta_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace milk_powder {

using meta = milk_powder_meta_t;
using message = milk_powder_message_t;

} // namespace milk_powder

struct milk_powder_meta_t : tea::mask_type<milk_powder::meta> {
  static
  auto init(uint32_t delta, uint8_t type, uint8_t args[], uint32_t len) -> milk_powder::meta * {
    return milk_powder_meta_create(delta, type, args, len);
  }
  auto drop() && -> void {
    milk_powder_meta_destroy(get());
  }
  auto type() const -> uint8_t {
    return milk_powder_meta_type(get());
  }
  auto args() const -> tea::slice<const uint8_t> {
    const uint8_t *ptr = nullptr;
    auto len = milk_powder_meta_args(get(), &ptr);
    return tea::slice<const uint8_t>(ptr, len);
  }
  static
  auto from_message(milk_powder::message *&&it) -> milk_powder::meta * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return milk_powder_meta_from_message(it);
  }
  auto to_message() const-> const milk_powder::message * {
    return milk_powder_meta_to_message(get());
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_META_H_
