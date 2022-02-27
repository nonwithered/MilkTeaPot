#ifndef LIB_TEA_REF_H_
#define LIB_TEA_REF_H_

#ifdef __cplusplus

#include <memory>
#include <tuple>

#include <tea/def.h>
#include <tea/cast.h>

#define TEA_RENAME(P, S, F) TEA_API decltype(F) P##_##S = F
#define TEA_FUNC(P, S) TEA_RENAME(P, S, S)

namespace tea {

namespace meta {

template<typename class_type>
struct Class {
  using target_type = cast_pair_t<class_type>;
  template<typename ...args_type>
  static
  auto TEA_CALL create(args_type ...args) -> target_type * {
    auto *ref = class_type::init(std::forward<args_type>(args)...);
    if (ref == nullptr) {
      return nullptr;
    }
    return unwrap_cast(*ref);
  }
  static
  auto TEA_CALL destroy(target_type *obj) -> void {
    auto &ref = wrap_cast<class_type>(obj);
    delete &ref;
  }
  template<typename value_type>
  struct Field {
    using target_value_type = std::remove_cv_t<value_type>;
    template<value_type class_type:: *member>
    static
    auto TEA_CALL getter(const target_type *obj) -> target_value_type {
      const auto &ref = wrap_cast<const class_type>(obj);
      return ref.*member;
    }
    template<value_type class_type:: *member>
    static
    auto TEA_CALL setter(target_type *obj, target_value_type value) -> void {
      auto &ref = wrap_cast<class_type>(obj);
      ref.*member = value;
    }
  };
  template<typename result_type, typename ...args_type>
  struct Method {
    template<result_type (class_type:: *member)(args_type...) const>
    static
    auto TEA_CALL invoke(const target_type *obj, args_type ...args) -> result_type {
      const auto &ref = wrap_cast<const class_type>(obj);
      return (ref.*member)(std::forward<args_type>(args)...);
    }
    template<result_type (class_type:: *member)(args_type...)>
    static
    auto TEA_CALL invoke(target_type *obj, args_type ...args) -> result_type {
      auto &ref = wrap_cast<class_type>(obj);
      return (ref.*member)(std::forward<args_type>(args)...);
    }
  };
};

template<typename member_type>
struct field_type;

template<typename class_type_t, typename value_type_t>
struct field_type<value_type_t class_type_t:: *> {
  using class_type = class_type_t;
  using value_type = value_type_t;
  using member_type = value_type class_type:: *;
  using target_value_type = std::remove_cv_t<value_type>;
  template<member_type member,
           typename target_type = cast_pair_t<class_type>>
  static
  auto TEA_CALL getter(const target_type *obj) -> target_value_type {
    return Class<class_type>::template Field<value_type>::template getter<member>(obj);
  }
  template<member_type member,
           typename target_type = cast_pair_t<class_type>>
  static
  auto TEA_CALL setter(target_type *obj, target_value_type value) -> void {
    return Class<class_type>::template Field<value_type>::template setter<member>(obj, value);
  }
};

struct field_handle {
  template<auto member,
           typename member_type = decltype(member),
           typename field = field_type<member_type>,
           typename value_type = typename field::target_value_type,
           typename class_type = typename field::class_type,
           typename target_type = cast_pair_t<class_type>>
  static
  auto TEA_CALL getter(const target_type *obj) -> value_type {
    return field::template getter<member>(obj);
  }
  template<auto member,
           typename member_type = decltype(member),
           typename field = field_type<member_type>,
           typename value_type = typename field::target_value_type,
           typename class_type = typename field::class_type,
           typename target_type = cast_pair_t<class_type>>
  static
  auto TEA_CALL setter(target_type *obj, value_type value) -> void {
    return field::template setter<member>(obj, value);
  }
};

template<typename member_type>
struct method_type;

template<typename class_type_t, typename return_type_t, typename ...args_type_t>
struct method_type<return_type_t (class_type_t:: *)(args_type_t...)> {
  using class_type = class_type_t;
  using actual_class_type = class_type;
  using return_type = return_type_t;
  using member_type = return_type (class_type:: *)(args_type_t...);
  template<member_type member,
           typename target_type = cast_pair_t<class_type>>
  static
  auto TEA_CALL invoke(target_type *obj, args_type_t ...args) -> return_type {
    return Class<class_type>::template Method<return_type, args_type_t...>::template invoke<member>
    (obj, std::forward<args_type_t>(args)...);
  }
};

template<typename class_type_t, typename return_type_t, typename ...args_type_t>
struct method_type<return_type_t (class_type_t:: *)(args_type_t...) const> {
  using class_type = class_type_t;
  using actual_class_type = const class_type;
  using return_type = return_type_t;
  using member_type = return_type (class_type:: *)(args_type_t...) const;
  template<member_type member,
           typename target_type = cast_pair_t<class_type>>
  static
  auto TEA_CALL invoke(const target_type *obj, args_type_t ...args) -> return_type {
    return Class<class_type>::template Method<return_type, args_type_t...>::template invoke<member>
      (obj, std::forward<args_type_t>(args)...);
  }
};

template<typename member_type, typename args_type>
struct is_args_match {
  static constexpr bool value = false;
};

template<typename class_type, typename return_type, typename ...args_type>
struct is_args_match<return_type (class_type::*)(args_type...), std::tuple<args_type...>> {
  static constexpr bool value = true;
};

template<typename class_type, typename return_type, typename ...args_type>
struct is_args_match<return_type (class_type::*)(args_type...) const, std::tuple<args_type...>> {
  static constexpr bool value = true;
};

template<typename member_type, typename args_type>
inline constexpr bool is_args_match_v = is_args_match<member_type, args_type>::value;

template<typename ...args_type>
struct method_handle {
  template<auto member,
           typename member_type = decltype(member),
           typename = std::enable_if_t<is_args_match_v<member_type, std::tuple<args_type...>>>,
           typename method = method_type<member_type>,
           typename return_type = typename method::return_type,
           typename actual_class_type = typename method::actual_class_type,
           typename target_type = cast_pair_t<actual_class_type>>
  static
  auto TEA_CALL invoke(target_type *obj, args_type ...args) -> return_type {
    return method::template invoke<member>
      (obj, std::forward<args_type>(args)...);
  }
};

template<typename sign_type>
struct function_type;

template<typename return_type_t, typename ...args_type>
struct function_type<return_type_t(args_type...)> {
  using return_type = return_type_t;
  template<return_type(* func)(args_type...)>
  static
  auto TEA_CALL invoke(args_type ...args) -> return_type {
    return func(std::forward<args_type>(args)...);
  }
};

template<typename ...args_type>
struct function_handle {
  template<auto func,
           typename func_type = std::remove_pointer_t<decltype(func)>,
           typename function = function_type<func_type>,
           typename return_type = typename function::return_type,
           typename = std::enable_if_t<std::is_same_v<func_type, return_type(args_type...)>>>
  static
  auto TEA_CALL invoke(args_type ...args) -> return_type {
    return func(std::forward<args_type>(args)...);
  }
};

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_REF_H_
