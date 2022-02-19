#ifndef LIB_TEA_FUNC_H_
#define LIB_TEA_FUNC_H_

#include <tea/facade.h>
#include <tea/def.h>

#ifdef __cplusplus

#include <type_traits>
#include <functional>


namespace tea {

template<typename T, auto field,
         typename class_type = T,
         typename = typename std::enable_if<std::is_class_v<class_type>>::type,
         typename field_type = decltype(field),
         typename = typename std::enable_if<std::is_member_object_pointer_v<field_type>>::type>
struct field_type_of final : empty_class {
  using type = typename std::remove_pointer<
  typename std::remove_reference<
  decltype(((class_type *) nullptr)->*field)
  >::type>::type;
};

template<typename T,
         typename = typename std::enable_if<std::is_class_v<T>>::type,
         typename = typename std::enable_if<std::is_trivial_v<T>>::type,
         typename sign_t = typename T::sign_t,
         typename = typename std::enable_if<std::is_function_v<sign_t>>::type>
struct func_info final : empty_class {
  using class_type = T;
  using sign_type = sign_t;
  static constexpr auto capture = &class_type::capture;
  using capture_type = typename field_type_of<class_type, capture>::type;
  static constexpr auto invoke =  &class_type::invoke;
  using invoke_type = typename field_type_of<class_type, invoke>::type;
};

template<typename capture_type, typename sign_type>
struct func_adapter;

template<typename capture_type, typename result_type, typename ...args_type>
struct func_adapter<capture_type, result_type(args_type...)> final : empty_class {
  static
  result_type TEA_CALL invoke(capture_type *capture, args_type ...args) {
    return (*(std::function<result_type(args_type...)> *) capture)(std::forward<args_type>(args)...);
  }
};

template<typename T,
         typename info = func_info<T>>
auto type_cast(std::function<typename info::sign_type> &func) -> T {
  using capture_type = typename info::capture_type;
  using sign_type = typename info::sign_type;
  return T {
    .capture = (capture_type *) &func,
    .invoke = func_adapter<capture_type, sign_type>::invoke,
  };
}

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_FUNC_H_
