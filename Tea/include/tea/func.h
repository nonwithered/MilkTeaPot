#ifndef LIB_TEA_FUNC_H_
#define LIB_TEA_FUNC_H_

#ifdef __cplusplus

#include <type_traits>
#include <functional>

#include <tea/def.h>
#include <tea/ref.h>

namespace tea {

namespace meta {

template<typename T>
struct func_sign {
};

template<typename T>
using func_sign_t = typename func_sign<T>::type;

template<typename sign_type>
struct func_invoke {};

template<typename return_type, typename ...args_type>
struct func_invoke<return_type(args_type...)> {
  using func_type = std::function<return_type(args_type...)>;
  template<typename T>
  static
  auto TEA_CALL invoke(T *obj, args_type ...args) -> return_type {
    func_type &func = *reinterpret_cast<func_type *>(obj);
    return func(std::forward<args_type>(args)...);
  }
};

template<typename T, auto invoke = &T::invoke, auto capture = &T::capture,
         typename class_type = T,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         typename sign_type = func_sign_t<class_type>,
         typename lambda_type = std::function<sign_type>,
         typename capture_type = decltype(capture),
         typename capture_field = field_type<capture_type>,
         typename context_type = std::remove_pointer_t<typename capture_field::value_type>>
auto function_cast(lambda_type &function) -> T {
  auto obj = class_type{};
  obj.*invoke = &func_invoke<sign_type>::template invoke<context_type>;
  obj.*capture = reinterpret_cast<context_type *>(&function);
  return obj;
}

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_FUNC_H_
