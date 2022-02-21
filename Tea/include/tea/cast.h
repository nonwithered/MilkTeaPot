#ifndef LIB_TEA_CAST_H_
#define LIB_TEA_CAST_H_

#ifdef __cplusplus

#include <type_traits>
#include <utility>

namespace tea {

namespace meta {

template<typename T>
struct cast_pair;

template<typename T>
using cast_pair_t = typename cast_pair<T>::type;

template<typename T>
struct cast_pair<const T> {
  using type = const cast_pair_t<T>;
};

template<typename T, typename Y>
struct can_unwrap_to {
  static constexpr bool value = false;
};

template<typename T, typename Y>
inline constexpr bool can_unwrap_to_v = can_unwrap_to<T, Y>::value;

template<typename T>
struct can_unwrap_to<T &, cast_pair_t<T> *> {
  static constexpr bool value = true;
};

template<typename T, typename Y>
struct can_wrap_from {
  static constexpr bool value = false;
};

template<typename T, typename Y>
inline constexpr bool can_wrap_from_v = can_wrap_from<T, Y>::value;

template<typename T>
struct can_wrap_from<T &, cast_pair_t<T> *> {
  static constexpr bool value = true;
};

template<typename T, typename Y,
         typename = std::enable_if_t<can_unwrap_to_v<T &, Y *>>>
auto unwrap_to(T &it) -> Y * {
  return reinterpret_cast<Y *>(&it);
}
template<typename T, typename Y,
         typename = std::enable_if_t<can_wrap_from_v<T &, Y *>>>
auto wrap_from(Y *it) -> T & {
  return *reinterpret_cast<T *>(it);
}

template<typename T,
         typename class_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename = std::enable_if_t<std::is_lvalue_reference_v<T>>,
         typename target_type = cast_pair_t<class_type>>
auto unwrap_cast(T &&it) -> target_type * {
  return unwrap_to<class_type, target_type>(std::forward<T>(it));
}

template<typename Y, typename T,
         typename class_type = Y,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename pointer_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_pointer_v<pointer_type>>,
         typename target_type = std::remove_pointer_t<pointer_type>>
auto wrap_cast(T &&it) -> class_type & {
  return wrap_from<class_type, target_type>(std::forward<T>(it));
}

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_CAST_H_
