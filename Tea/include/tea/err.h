#ifndef LIB_TEA_ERR_H_
#define LIB_TEA_ERR_H_

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

TEA_API tea_err_type_t TEA_CALL
tea_err_type(const tea_err_t *);

TEA_API const char * TEA_CALL
tea_err_message(const tea_err_t *);

TEA_API const tea_err_t * TEA_CALL
tea_err_cause(const tea_err_t *);

TEA_API const tea_err_t * TEA_CALL
tea_err_suppressed(const tea_err_t *);

TEA_API bool TEA_CALL
tea_err_is(const tea_err_t *, tea_err_type_t);

TEA_API tea_err_t * TEA_CALL
tea_err_emit(tea_err_type_t, const char [], tea_err_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

#include <functional>

struct tea_err_t {
 private:
  tea_err_t() = delete;
  ~tea_err_t() = delete;
 public:
  static tea_err_t *init() {
    return tea_err_emit(nullptr, nullptr, nullptr);
  }
  void drop() {
    if (this == nullptr) {
      return;
    }
    assert(tea_err_emit(nullptr, nullptr, this) == nullptr);
  }
  tea_err_type_t type() const {
    return tea_err_type(this);
  }
  const char *message() const {
    return tea_err_message(this);
  }
  const tea_err_t *cause() const {
    return tea_err_cause(this);
  }
  const tea_err_t *suppressed() const {
    return tea_err_suppressed(this);
  }
  bool is(tea_err_type_t type) const {
    return tea_err_is(this, type);
  }
  template<tea_err_type_t type>
  static void raise(const char message[] = nullptr, tea_err_t *e = nullptr) {
    static_assert(type != nullptr, "type != nullptr");
    assert(tea_err_emit(type, message, e) == nullptr);
  }
  template<tea_err_type_t type>
  tea_err_t *check(std::function<tea_err_t *(tea_err_t &)> block) {
    if (this == nullptr) {
      return nullptr;
    }
    if (!is(type)) {
      return this;
    }
    return block(*this);
  }
};

namespace tea {
  using err = ::tea_err_t;
  using err_meta = ::tea_err_meta_t;
  using err_type = ::tea_err_type_t;
} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_ERR_H_
