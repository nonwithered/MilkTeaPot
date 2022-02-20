#ifndef TEA_ERR_H_
#define TEA_ERR_H_

#include <utility>
#include <sstream>
#include <string>

#include <tea.h>

namespace tea {

struct Err;

template<>
struct meta::unwrap_pair<Err> {
  using type = err;
};

template<>
struct meta::wrap_pair<err> {
  using type = Err;
};

using namespace meta;

struct Err final {
  const err_type type_;
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
  auto cause() const -> const err * {
    return unwrap_cast(*cause_);
  }
  auto suppressed() const -> const err * {
    return unwrap_cast(*suppressed_);
  }
  auto is(err_type type) const -> bool {
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
  auto TEA_CALL emit(err_type type, const char what[], err *cause) -> err * {
    auto *e = cell();
    if (type != nullptr) {
      cell() = new Err {
          .type_ = type,
          .what_ = what == nullptr ? "" : what,
          .cause_ = &wrap_cast(cause),
          .suppressed_ = e,
      };
      return nullptr;
    }
    if (cause != nullptr) {
      delete &wrap_cast(cause);
      return nullptr;
    }
    if (e == nullptr) {
      return nullptr;
    }
    cause = unwrap_cast(*e);
    cell() = nullptr;
    return cause;
  }
 private:
  auto str() const -> std::string {
    std::stringstream ss;
    dump_to(ss, 0, true);
    return ss.str();
  }
  auto dump_to(std::stringstream &ss, size_t n, bool recusive) const -> void {
    ss << type_().name_;
    if (!what_.empty()) {
      ss << ": " << what_;
    }
    ss << "\n";
    if (cause_ != nullptr) {
      space(ss, n + 1);
      ss << "Caused by: ";
      cause_->dump_to(ss, n + 1, true);
    }
    if (!recusive) {
      return;
    }
    for (Err *it = suppressed_; it != nullptr; it = it->suppressed_) {
      space(ss, n);
      ss << "Suppressed: ";
      it->dump_to(ss, n, false);
    }
  }
  static
  auto space(std::stringstream &ss, size_t n) -> void {
    for (size_t i = 0; i != n; ++i) {
      ss << "  ";
    }
  }
  static
  auto cell() -> Err *&;
};

} // namespace tea

#endif // ifndef TEA_ERR_H_
