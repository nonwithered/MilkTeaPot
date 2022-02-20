#ifndef LIB_TEA_MASK_H_
#define LIB_TEA_MASK_H_

#ifdef __cplusplus

#include <type_traits>
#include <memory>

#include <tea/life.h>

namespace tea {
template<typename T,
         typename class_type = T,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename = std::enable_if_t<!std::is_destructible_v<class_type>>>
struct Drop {
//  auto drop() && -> void;
};

template<typename T,
         typename = std::enable_if_t<std::is_class_v<T>>>
class mask_type : empty_class, Drop<T> {
 protected:
  auto get() -> T * {
    return (T *) this;
  }
  auto get() const -> const T * {
    return (T *) this;
  }
};

template<typename T,
         typename pointer_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_pointer_v<pointer_type>>,
         typename class_type = std::remove_pointer_t<pointer_type>,
         typename = std::enable_if_t<std::is_base_of_v<Drop<class_type>, class_type>>,
         auto drop_method = &class_type::drop,
         typename drop_method_type = decltype(drop_method),
         typename = std::enable_if_t<std::is_member_function_pointer_v<drop_method_type>>,
         typename = std::enable_if_t<std::is_same_v<drop_method_type, void (class_type:: *)() &&>>>
auto drop(T &&ptr) -> void {
  if (ptr == nullptr) {
    return;
  }
  (std::move(*ptr).*drop_method)();
  ptr = nullptr;
}

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_MASK_H_
