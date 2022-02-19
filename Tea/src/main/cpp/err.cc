#include <utility>
#include <sstream>
#include <string>

#include <tea.h>

namespace tea {

struct Err final {
  using cast_type = tea::err_t *;
  using const_cast_type = const tea::err_t *;
  static
  auto type_cast(const err_t *it) -> const Err & {
    return *reinterpret_cast<const Err *>(it);
  }
  static
  auto type_cast(err_t *it) -> Err & {
    return *reinterpret_cast<Err *>(it);
  }
  static
  auto type_cast(const Err &it) -> const err_t * {
    return reinterpret_cast<const err_t *>(&it);
  }
  static
  auto type_cast(Err &it) -> err_t * {
    return reinterpret_cast<err_t *>(&it);
  }
  const err_type_t type_;
  const std::string what_;
  Err *const cause_;
  Err *const suppressed_;
  ~Err() {
    delete suppressed_;
    delete cause_;
  }
  bool is(err_type_t type) const {
    if (type == nullptr) {
      return true;
    }
    for (auto it = type_; it != nullptr; it = it().super_) {
      if (it == type) {
        return true;
      }
    }
    return false;
  }
  std::string str() const {
    std::stringstream ss;
    str(ss, 0);
    return ss.str();
  }
 private:
  void str(std::stringstream &ss, size_t n) const {
    ++n;
    ss << type_().name_;
    if (!what_.empty()) {
      ss << ": " << what_;
    }
    ss << "\n";
    if (cause_ != nullptr) {
      space(ss, n);
      ss << "Caused by: ";
      cause_->str(ss, n);
    }
    for (Err *it = suppressed_; it != nullptr; it = it->suppressed_) {
      space(ss, n);
      ss << "Suppressed: ";
      it->str(ss, n);
    }
  }
  static void space(std::stringstream &ss, size_t n) {
    for (size_t i = 0; i != n; ++i) {
      ss << "  ";
    }
  }
};

} // namespace tea

using namespace tea;
using namespace tea::meta;

extern "C" {

TEA_API tea_err_t * TEA_CALL
tea_err_emit(tea_err_type_t type, const char what[], tea_err_t *cause) {
  thread_local Err *e = nullptr;
  if (type != nullptr) {
    e = new Err {
      .type_ = type,
      .what_ = what == nullptr ? "" : what,
      .cause_ = &Err::type_cast(cause),
      .suppressed_ = e,
    };
    return nullptr;
  }
  if (cause != nullptr) {
    delete &Err::type_cast(cause);
    return nullptr;
  }
  if (e == nullptr) {
    return nullptr;
  }
  cause = Err::type_cast(*e);
  e = nullptr;
  return cause;
}

TEA_API tea_err_type_t TEA_CALL
tea_err_type(const tea_err_t *obj) {
  return Class<Err>::Field<const err_type_t>::get<&Err::type_>(obj);
}

TEA_API const char * TEA_CALL
tea_err_what(const tea_err_t *obj) {
  return Class<Err>::Field<const std::string>::get<&Err::what_>(obj).data();
}

TEA_API const tea_err_t * TEA_CALL
tea_err_cause(const tea_err_t *obj) {
  return Err::type_cast(*Class<Err>::Field<Err *const>::get<&Err::cause_>(obj));
}

TEA_API const tea_err_t * TEA_CALL
tea_err_suppressed(const tea_err_t *obj) {
  return Err::type_cast(*Class<Err>::Field<Err *const>::get<&Err::suppressed_>(obj));
}

TEA_API bool TEA_CALL
tea_err_is(const tea_err_t *obj, tea_err_type_t type) {
  return Class<Err>::Method<bool, err_type_t>::invoke<&Err::is>(obj, type);
}

TEA_API void TEA_CALL
tea_err_str(const tea_err_t *obj, tea_err_dump_recv_t recv) {
  auto str = Err::type_cast(obj).str();
  recv.invoke(recv.capture, str.data(), str.size());
}

} // extern "C"
