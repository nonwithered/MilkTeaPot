#ifndef LIB_TEA_CAST_H_
#define LIB_TEA_CAST_H_

#ifdef __cplusplus

#include <type_traits>

#include <tea/life.h>

namespace tea {

template<typename T>
struct type_pair {
};

template<typename T>
using type_pair_t = typename type_pair<T>::type;

template<typename T>
struct type_link {
};

template<typename T>
struct type_link<T &> {
  using type = type_pair_t<T> *;
};

template<typename T>
struct type_link<const T &> {
  using type = const type_pair_t<T> *;
};

template<typename T>
struct type_link<T *> {
  using type = type_pair_t<T> &;
};

template<typename T>
struct type_link<const T *> {
  using type = const type_pair_t<T> &;
};

template<typename T>
using type_link_t = typename type_link<T>::type;

template<typename T,
         typename lvalue_reference_type = T,
         typename = std::enable_if_t<std::is_lvalue_reference_v<lvalue_reference_type>>,
         typename class_type = std::remove_reference_t<lvalue_reference_type>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename = std::enable_if_t<std::is_destructible_v<class_type>>,
         typename link_type = type_link_t<lvalue_reference_type>,
         typename pointer_type = link_type,
         typename = std::enable_if_t<std::is_pointer_v<pointer_type>>,
         typename struct_type = std::remove_pointer_t<pointer_type>,
         typename = std::enable_if_t<std::is_class_v<struct_type>>,
         typename = std::enable_if_t<!std::is_destructible_v<struct_type>>>
auto unwrap_cast(T &&it) -> link_type {
  return reinterpret_cast<struct_type *>(&it);
};

template<typename T,
         typename pointer_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_pointer_v<pointer_type>>,
         typename struct_type = std::remove_pointer_t<pointer_type>,
         typename = std::enable_if_t<std::is_class_v<struct_type>>,
         typename = std::enable_if_t<!std::is_destructible_v<struct_type>>,
         typename link_type = type_link_t<pointer_type>,
         typename lvalue_reference_type = link_type,
         typename = std::enable_if_t<std::is_lvalue_reference_v<lvalue_reference_type>>,
         typename class_type = std::remove_reference_t<lvalue_reference_type>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename = std::enable_if_t<std::is_destructible_v<class_type>>>
auto wrap_cast(T &&it) -> link_type {
  return *reinterpret_cast<class_type *>(it);
};

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_CAST_H_
