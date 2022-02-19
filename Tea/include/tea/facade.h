#ifndef LIB_TEA_FACADE_H_
#define LIB_TEA_FACADE_H_

#ifdef __cplusplus

#include <type_traits>
#include <memory>

namespace tea {

class remove_constructor {
  remove_constructor() = delete;
};

class remove_destructor {
  ~remove_destructor() = delete;
};

template<typename T>
class remove_copy {
  remove_copy(const T &) = delete;
  auto operator=(const T &) -> void = delete;
};

template<typename T>
class remove_move {
  remove_move(T &&) = delete;
  auto operator=(T &&) -> void = delete;
};

template<typename T>
class remove_assign {
  auto operator=(const T &) -> void = delete;
  auto operator=(T &&) -> void = delete;
};

class empty_class : remove_constructor, remove_destructor {
};

template<typename T,
         typename = typename std::enable_if<std::is_class_v<T>>::type>
class facade_type : empty_class {
 protected:
  auto get() -> T * {
    return (T *) this;
  }
  auto get() const -> const T * {
    return (T *) this;
  }
};

template<typename T,
         typename pointer_type = typename std::remove_reference<T>::type,
         typename = typename std::enable_if<std::is_pointer_v<pointer_type>>::type,
         typename class_type = typename std::remove_pointer<pointer_type>::type,
         typename = typename std::enable_if<std::is_class_v<class_type>>::type,
         auto drop_method = &class_type::drop,
         typename drop_method_type = decltype(drop_method),
         typename = typename std::enable_if<std::is_member_function_pointer_v<drop_method_type>>::type,
         typename = typename std::enable_if<std::is_same_v<drop_method_type, void (class_type:: *)() &&>>::type>
auto drop(T &&ptr) -> void {
  if (ptr == nullptr) {
    return;
  }
  (std::move(*ptr).*drop_method)();
  ptr = nullptr;
}

template<typename FROM, typename TO>
auto type_cast(FROM) -> TO = delete;

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_FACADE_H_
