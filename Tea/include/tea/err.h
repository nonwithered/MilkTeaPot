#ifndef LIB_TEA_ERR_H_
#define LIB_TEA_ERR_H_

#ifdef __cplusplus
#include <functional>
#include <string>
#include <tea/func.h>
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

struct tea_err_t;

TEA_API tea_err_t * TEA_CALL
tea_err_emit(tea_err_type_t, const char [], tea_err_t *);

TEA_API tea_err_type_t TEA_CALL
tea_err_type(const tea_err_t *);

TEA_API const char * TEA_CALL
tea_err_what(const tea_err_t *);

TEA_API const tea_err_t * TEA_CALL
tea_err_cause(const tea_err_t *);

TEA_API const tea_err_t * TEA_CALL
tea_err_suppressed(const tea_err_t *);

TEA_API bool TEA_CALL
tea_err_is(const tea_err_t *, tea_err_type_t);

struct tea_err_dump_recv_t {
  struct tea_err_dump_recv_capture_t *capture;
  void (TEA_CALL *invoke)(struct tea_err_dump_recv_capture_t *, const char [], size_t);
#ifdef __cplusplus
  using sign_t = void(const char [], size_t);
#endif // ifdef __cplusplus
};

TEA_API void TEA_CALL
tea_err_str(const tea_err_t *, tea_err_dump_recv_t);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace tea {
  using err_t = tea_err_t;
  using err_meta_t = tea_err_meta_t;
  using err_type_t = tea_err_type_t;
} // namespace tea

struct tea_err_t : tea::facade_type<tea::err_t> {
  static auto init() -> tea::err_t * {
    return tea_err_emit(nullptr, nullptr, nullptr);
  }
  auto drop() && -> void {
    assert(tea_err_emit(nullptr, nullptr, get()) == nullptr);
  }
  auto type() const -> tea::err_type_t {
    return tea_err_type(get());
  }
  auto what() const -> const char * {
    return tea_err_what(get());
  }
  auto cause() const -> const tea::err_t * {
    return tea_err_cause(get());
  }
  auto suppressed() const -> const tea::err_t * {
    return tea_err_suppressed(get());
  }
  auto is(tea::err_type_t type) const -> bool {
    return tea_err_is(get(), type);
  }
  auto str(std::function<void(const char [], size_t)> func) const -> void {
    return tea_err_str(get(), tea::type_cast<tea_err_dump_recv_t>(func));
  }
  auto str() const -> std::string {
    std::string s;
    str([&s](auto what, auto size) {
      s = std::string(what, size);
    });
    return s;
  }
  template<tea::err_type_t type>
  auto check(std::function<tea::err_t *(tea::err_t &)> block) -> tea::err_t * {
    if (get() == nullptr) {
      return nullptr;
    }
    if (!is(type)) {
      return get();
    }
    return block(*get());
  }
  template<tea::err_type_t type,
           typename = typename std::enable_if<type != nullptr>::type>
  static auto raise(const char what[] = nullptr, tea::err_t *e = nullptr) -> void {
    assert(tea_err_emit(type, what, e) == nullptr);
  }
};

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_ERR_H_
