#ifndef LIB_TEA_CAST_H_
#define LIB_TEA_CAST_H_

#ifdef __cplusplus

#include <type_traits>

#include <tea/life.h>

namespace tea {

namespace meta {

template<typename T>
struct unwrap_pair {
};

template<typename T>
using unwrap_pair_t = typename unwrap_pair<T>::type;

template<typename T>
struct unwrap_pair<T &> {
  using type = unwrap_pair_t<T> *;
};

template<typename T>
struct unwrap_pair<const T &> {
  using type = const unwrap_pair_t<T> *;
};

template<typename T>
struct wrap_pair {
};

template<typename T>
using wrap_pair_t = typename wrap_pair<T>::type;

template<typename T>
struct wrap_pair<T *> {
  using type = wrap_pair_t<T> &;
};

template<typename T>
struct wrap_pair<const T *> {
  using type = const wrap_pair_t<T> &;
};

template<typename T,
         typename lvalue_reference_type = T,
         typename = std::enable_if_t<std::is_lvalue_reference_v<lvalue_reference_type>>,
         typename class_type = std::remove_reference_t<lvalue_reference_type>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename = std::enable_if_t<std::is_destructible_v<class_type>>,
         typename unwrap_type = unwrap_pair_t<lvalue_reference_type>,
         typename pointer_type = unwrap_type,
         typename = std::enable_if_t<std::is_pointer_v<pointer_type>>,
         typename struct_type = std::remove_pointer_t<pointer_type>,
         typename = std::enable_if_t<std::is_class_v<struct_type>>,
         typename = std::enable_if_t<!std::is_destructible_v<struct_type>>>
auto unwrap_cast(T &&it) -> unwrap_type {
  return reinterpret_cast<struct_type *>(&it);
};

template<typename T,
         typename pointer_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_pointer_v<pointer_type>>,
         typename struct_type = std::remove_pointer_t<pointer_type>,
         typename = std::enable_if_t<std::is_class_v<struct_type>>,
         typename = std::enable_if_t<!std::is_destructible_v<struct_type>>,
         typename wrap_type = wrap_pair_t<pointer_type>,
         typename lvalue_reference_type = wrap_type,
         typename = std::enable_if_t<std::is_lvalue_reference_v<lvalue_reference_type>>,
         typename class_type = std::remove_reference_t<lvalue_reference_type>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename = std::enable_if_t<std::is_destructible_v<class_type>>>
auto wrap_cast(T &&it) -> wrap_type {
  return *reinterpret_cast<class_type *>(it);
};

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_CAST_H_
