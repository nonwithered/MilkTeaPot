#ifndef LIB_TEA_ERR_H_
#define LIB_TEA_ERR_H_

#ifdef __cplusplus
#include <functional>
#include <string>
#include <tea/func.h>
#include <tea/mask.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

struct tea_err_meta_t;

typedef struct tea_err_meta_t (TEA_CALL *tea_err_type_t)();

struct tea_err_meta_t {
  tea_err_type_t super_;
  const char *name_;
};

#define TEA_ERR_ENUM_IMPL(NAME, SUPER) \
TEA_API struct tea_err_meta_t TEA_CALL \
NAME() { \
  static const char STR[] = #NAME; \
  return { SUPER, STR }; \
}

#define TEA_ERR_ENUM_DECL(NAME) \
TEA_API struct tea_err_meta_t TEA_CALL NAME();

struct tea_err_t;

struct tea_err_dump_recv_t {
  struct tea_err_dump_recv_capture_t *capture;
  void (TEA_CALL *invoke)(struct tea_err_dump_recv_capture_t *, const char [], size_t);
};

extern
TEA_API tea_err_t * (TEA_CALL *
tea_err_emit)(tea_err_type_t, const char [], tea_err_t *);

extern
TEA_API tea_err_type_t (TEA_CALL *
tea_err_type_of)(const tea_err_t *);

extern
TEA_API const char * (TEA_CALL *
tea_err_what)(const tea_err_t *);

extern
TEA_API const tea_err_t * (TEA_CALL *
tea_err_cause)(const tea_err_t *);

extern
TEA_API const tea_err_t * (TEA_CALL *
tea_err_suppressed)(const tea_err_t *);

extern
TEA_API bool (TEA_CALL *
tea_err_is)(const tea_err_t *, tea_err_type_t);

extern
TEA_API void (TEA_CALL *
tea_err_dump)(const tea_err_t *, tea_err_dump_recv_t);

TEA_ERR_ENUM_DECL(tea_err_type_bad_logic) // <- nullptr
TEA_ERR_ENUM_DECL(tea_err_type_invalid_param) // <- tea_err_type_bad_logic
TEA_ERR_ENUM_DECL(tea_err_type_null_obj) // <- tea_err_type_invalid_param
TEA_ERR_ENUM_DECL(tea_err_type_out_of_range) // <- tea_err_type_invalid_param
TEA_ERR_ENUM_DECL(tea_err_type_cast_failure) // <- tea_err_type_invalid_param
TEA_ERR_ENUM_DECL(tea_err_type_invalid_state) // <- tea_err_type_bad_logic
TEA_ERR_ENUM_DECL(tea_err_type_assertion) // <- tea_err_type_invalid_state
TEA_ERR_ENUM_DECL(tea_err_type_runtime_warn) // <- nullptr
TEA_ERR_ENUM_DECL(tea_err_type_unsupported) // <- tea_err_type_runtime_warn
TEA_ERR_ENUM_DECL(tea_err_type_io_failure) // <- tea_err_type_runtime_warn
TEA_ERR_ENUM_DECL(tea_err_type_unexpected_eof) // <- tea_err_type_io_failure
TEA_ERR_ENUM_DECL(tea_err_type_undefined_format) // <- tea_err_type_io_failure

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace tea {

using err = tea_err_t;
using err_meta = tea_err_meta_t;
using err_type = tea_err_type_t;

namespace err_enum {

inline constexpr auto bad_logic = tea_err_type_bad_logic;
inline constexpr auto invalid_param = tea_err_type_invalid_param;
inline constexpr auto null_obj = tea_err_type_null_obj;
inline constexpr auto out_of_range = tea_err_type_out_of_range;
inline constexpr auto cast_failure = tea_err_type_cast_failure;
inline constexpr auto invalid_state = tea_err_type_invalid_state;
inline constexpr auto assertion = tea_err_type_assertion;
inline constexpr auto runtime_warn = tea_err_type_runtime_warn;
inline constexpr auto unsupported = tea_err_type_unsupported;
inline constexpr auto io_failure = tea_err_type_io_failure;
inline constexpr auto unexpected_eof = tea_err_type_unexpected_eof;
inline constexpr auto undefined_format = tea_err_type_undefined_format;

} // namespace err_enum
} // namespace tea

template<>
struct tea::meta::func_sign<tea_err_dump_recv_t> {
  using type = void(const char *, size_t);
};

struct tea_err_t : tea::mask_type<tea::err> {
  static
  auto init() -> tea::err * {
    return tea_err_emit(nullptr, nullptr, nullptr);
  }
  auto drop() && -> void {
    assert(tea_err_emit(nullptr, nullptr, get()) == nullptr);
  }
  auto type() const -> tea::err_type {
    return tea_err_type_of(get());
  }
  auto what() const -> const char * {
    return tea_err_what(get());
  }
  auto cause() const -> const tea::err * {
    return tea_err_cause(get());
  }
  auto suppressed() const -> const tea::err * {
    return tea_err_suppressed(get());
  }
  auto is(tea::err_type type) const -> bool {
    return tea_err_is(get(), type);
  }
  auto dump(std::function<void(const char [], size_t)> func) const -> void {
    return tea_err_dump(get(), tea::meta::function_cast<tea_err_dump_recv_t>(func));
  }
  auto str() const -> std::string {
    std::string s;
    dump([&s](auto what, auto size) {
      s = std::string(what, size);
    });
    return s;
  }
  template<tea::err_type type>
  auto check(std::function<tea::err *(tea::err &)> block) -> tea::err * {
    if (get() == nullptr) {
      return nullptr;
    }
    if (!is(type)) {
      return get();
    }
    return block(*get());
  }
  template<tea::err_type type,
           typename = std::enable_if_t<type != nullptr>>
  static
  auto raise(const char what[] = nullptr, tea::err *e = nullptr) -> void {
    assert(tea_err_emit(type, what, e) == nullptr);
  }
  template<tea::err_type type>
  static
  auto raise(std::function<std::string()> func, tea::err *e = nullptr) -> void {
    auto what = func();
    raise<type>(what.data(), e);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_ERR_H_
