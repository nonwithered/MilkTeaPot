#ifndef LIB_MILK_POWDER_EVENT_H_
#define LIB_MILK_POWDER_EVENT_H_

#ifdef __cplusplus
#include <array>
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct milk_powder_message_t;

struct milk_powder_event_t;

extern
TEA_API struct milk_powder_event_t * (TEA_CALL *
milk_powder_event_create)(uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1);

extern
TEA_API void (TEA_CALL *
milk_powder_event_destroy)(struct milk_powder_event_t *);

extern
TEA_API uint8_t (TEA_CALL *
milk_powder_event_type)(const struct milk_powder_event_t *);

extern
TEA_API uint8_t (TEA_CALL *
milk_powder_event_args)(const struct milk_powder_event_t *, uint8_t *);

extern
TEA_API struct milk_powder_event_t * (TEA_CALL *
milk_powder_event_from_message)(struct milk_powder_message_t *);

extern
TEA_API const struct milk_powder_message_t * (TEA_CALL *
milk_powder_event_to_message)(const struct milk_powder_event_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace milk_powder {

using event = milk_powder_event_t;
using message = milk_powder_message_t;

} // namespace milk_powder

struct milk_powder_event_t : tea::mask_type<milk_powder::event> {
  static
  auto init(uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1) -> milk_powder::event * {
    return milk_powder_event_create(delta, type, arg0, arg1);
  }
  auto drop() && -> void {
    milk_powder_event_destroy(get());
  }
  auto type() const -> uint8_t {
    return milk_powder_event_type(get());
  }
  auto args() const -> std::array<uint8_t, 2> {
    std::array<uint8_t, 2> arr;
    arr[0] = milk_powder_event_args(get(), &arr[1]);
    return arr;
  }
  static
  auto from_message(milk_powder::message *&&it) -> milk_powder::event * {
    tea::defer d = [&it]() {
      it = nullptr;
    };
    return milk_powder_event_from_message(it);
  }
  auto to_message() const-> const milk_powder::message * {
    return milk_powder_event_to_message(get());
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILK_POWDER_EVENT_H_
