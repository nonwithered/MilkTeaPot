#ifndef LIB_TEA_FUNC_H_
#define LIB_TEA_FUNC_H_

#ifdef __cplusplus

#include <tea/def.h>
#include <tea/facade.h>

#include <type_traits>
#include <functional>

namespace tea {

namespace meta {

template<typename T>
struct func_sign {
};

template<typename T>
using func_sign_t = typename func_sign<T>::type;

template<typename T, auto field,
         typename class_type = T,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename field_type = decltype(field),
         typename = std::enable_if_t<std::is_member_object_pointer_v<field_type>>>
struct field_type_of {
  using type = std::remove_pointer_t<std::remove_reference_t<decltype(((class_type *) nullptr)->*field)>>;
};

template<typename T, auto field>
using field_type_of_t = typename field_type_of<T, field>::type;

template<typename T,
         typename = std::enable_if_t<std::is_class_v<T>>,
         typename = std::enable_if_t<std::is_trivial_v<T>>,
         typename = std::enable_if_t<std::is_function_v<func_sign_t<T>>>>
struct func_info final {
  using class_type = T;
  using sign_type = func_sign_t<T>;
  static constexpr auto capture = &class_type::capture;
  using capture_type = field_type_of_t<class_type, capture>;
  static constexpr auto invoke =  &class_type::invoke;
  using invoke_type = field_type_of_t<class_type, invoke>;
};

template<typename capture_type, typename sign_type>
struct func_handle;

template<typename capture_type, typename result_type, typename ...args_type>
struct func_handle<capture_type, result_type(args_type...)> {
  static auto TEA_CALL invoke(capture_type *capture, args_type ...args) -> result_type{
    return (*(std::function<result_type(args_type...)> *) capture)(std::forward<args_type>(args)...);
  }
};

} // namespace meta

template<typename T,
         typename info = meta::func_info<T>>
auto func_cast(std::function<typename info::sign_type> &func) -> T {
  using capture_type = typename info::capture_type;
  using sign_type = typename info::sign_type;
  return T {
    .capture = (capture_type *) &func,
    .invoke = meta::func_handle<capture_type, sign_type>::invoke,
  };
}

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_FUNC_H_
