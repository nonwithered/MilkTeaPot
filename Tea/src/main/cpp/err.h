#ifndef TEA_ERR_H_
#define TEA_ERR_H_

#include <utility>
#include <sstream>
#include <string>

#include <tea.h>

namespace tea {

struct Err;

namespace meta {

template<>
struct unwrap_pair<Err> {
  using type = err_t;
};

template<>
struct wrap_pair<err_t> {
  using type = Err;
};

} // namespace meta

using namespace meta;

struct Err final {
  using raw_type = err_t *;
  using const_raw_type = const err_t *;
  const err_type_t type_;
  const std::string what_;
  Err *const cause_;
  Err *const suppressed_;
  ~Err() {
    delete suppressed_;
    delete cause_;
  }
  auto what() const -> const char * {
    return what_.data();
  }
  auto cause() const -> const err_t * {
    return unwrap_cast(*cause_);
  }
  auto is(err_type_t type) const -> bool {
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
  auto dump(tea_err_dump_recv_t recv) const -> void {
    auto s = str();
    recv.invoke(recv.capture, s.data(), s.size());
  }
  static
  auto emit(err_type_t type, const char what[], err_t *cause) -> tea_err_t * {
    if (type != nullptr) {
      cell() = new Err {
          .type_ = type,
          .what_ = what == nullptr ? "" : what,
          .cause_ = &wrap_cast(cause),
          .suppressed_ = cell(),
    };
      return nullptr;
    }
    if (cause != nullptr) {
      delete &wrap_cast(cause);
      return nullptr;
    }
    if (cell() == nullptr) {
      return nullptr;
    }
    cause = unwrap_cast(*cell());
    cell() = nullptr;
    return cause;
  }
 private:
  auto str() const -> std::string {
    std::stringstream ss;
    dump(ss, 0);
    return ss.str();
  }
  void dump(std::stringstream &ss, size_t n) const {
    ++n;
    ss << type_().name_;
    if (!what_.empty()) {
      ss << ": " << what_;
    }
    ss << "\n";
    if (cause_ != nullptr) {
      space(ss, n);
      ss << "Caused by: ";
      cause_->dump(ss, n);
    }
    for (Err *it = suppressed_; it != nullptr; it = it->suppressed_) {
      space(ss, n);
      ss << "Suppressed: ";
      it->dump(ss, n);
    }
  }
  static
  auto space(std::stringstream &ss, size_t n) -> void {
    for (size_t i = 0; i != n; ++i) {
      ss << "  ";
    }
  }
  static Err *&cell();
};

} // namespace tea

#endif // ifndef TEA_ERR_H_
