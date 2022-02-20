#ifndef LIB_TEA_LIFE_H_
#define LIB_TEA_LIFE_H_

#ifdef __cplusplus

#include <functional>
#include <memory>

namespace tea {

class remove_ctor {
  remove_ctor() = delete;
};

class remove_dtor {
  ~remove_dtor() = delete;
 protected:
  remove_dtor() = default;
};

class remove_copy {
  remove_copy(const remove_copy &) = delete;
 protected:
  remove_copy() = default;
  remove_copy(remove_copy &&) = default;
};

class remove_move : protected remove_copy {
  remove_move(remove_move &&) = delete;
 protected:
  remove_move() = default;
};

class empty_class : remove_move, remove_ctor, remove_dtor {
};

struct defer final : remove_move {
  template<typename func_type>
  defer(func_type func) : func_(std::move(func)) {}
  ~defer() {
    auto func = std::move(func_);
    if (!func) {
      return;
    }
    func();
  }
 private:
  std::function<void()> func_;
};

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_LIFE_H_
