#ifndef LIB_TEA_REF_H_
#define LIB_TEA_REF_H_

#ifdef __cplusplus

#include <memory>
#include <tuple>

#include <tea/def.h>
#include <tea/facade.h>
#include <tea/cast.h>

#define TEA_FUNC_LINK(S, F) \
TEA_API decltype(F) * S = F

namespace tea {

namespace meta {

template<typename class_type>
struct Class {
  using unwrap_type = unwrap_pair_t<class_type> *;
  using const_unwrap_type = const unwrap_pair_t<class_type> *;
  template<typename ...args_type>
  static
  auto TEA_CALL create(args_type ...args) -> unwrap_type {
    auto &ref = *std::make_unique<class_type>(std::forward<args_type>(args)...).release();
    return unwrap_cast(ref);
  }
  static
  auto TEA_CALL destroy(unwrap_type obj) -> void {
    auto &ref = wrap_cast(std::forward<unwrap_type>(obj));
    drop(std::move(ref));
  }
  template<typename value_type>
  struct Field {
    using value_type_unwrap = std::remove_cv_t<value_type>;
    template<value_type class_type:: *member>
    static
    auto TEA_CALL getter(const_unwrap_type obj) -> value_type_unwrap {
      const auto &ref = wrap_cast(std::forward<const_unwrap_type>(obj));
      return ref.*member;
    }
    template<value_type class_type:: *member>
    static
    auto TEA_CALL setter(unwrap_type obj, value_type_unwrap value) -> void {
      auto &ref = wrap_cast(std::forward<unwrap_type>(obj));
      ref.*member = std::forward(value);
    }
  };
  template<typename result_type, typename ...args_type>
  struct Method {
    template<result_type (class_type:: *member)(args_type...) const>
    static
    auto TEA_CALL invoke(const_unwrap_type obj, args_type ...args) -> result_type {
      const auto &ref = wrap_cast(std::forward<const_unwrap_type>(obj));
      return (ref.*member)(std::forward<args_type>(args)...);
    }
    template<result_type (class_type:: *member)(args_type...)>
    static
    auto TEA_CALL invoke(unwrap_type obj, args_type ...args) -> result_type {
      auto &ref = wrap_cast(std::forward<unwrap_type>(obj));
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
  using unwrap_type = unwrap_pair_t<class_type &>;
  using const_unwrap_type = unwrap_pair_t<const class_type &>;
  using value_type_unwrap = std::remove_cv_t<value_type>;
  template<member_type member>
  static
  auto TEA_CALL getter(const_unwrap_type obj) -> value_type_unwrap {
    return Class<class_type>::template Field<value_type>::template getter<member>
      (std::forward<const_unwrap_type>(obj));
  }
  template<member_type member>
  static
  auto TEA_CALL setter(unwrap_type obj, value_type_unwrap value) -> void {
    return Class<class_type>::template Field<value_type>::template setter<member>
      (std::forward<unwrap_type>(obj), std::forward<value_type_unwrap>(value));
  }
};

struct field_handle {
  template<auto member,
           typename member_type = decltype(member),
           typename field = field_type<member_type>,
           typename value_type = typename field::value_type_unwrap,
           typename const_unwrap_type = typename field::const_unwrap_type>
  static
  auto TEA_CALL getter(const_unwrap_type obj) -> value_type {
    return field::template getter<member>
      (std::forward<const_unwrap_type>(obj));
  }
  template<auto member,
           typename member_type = decltype(member),
           typename field = field_type<member_type>,
           typename value_type = typename field::value_type_unwrap,
           typename unwrap_type = typename field::unwrap_type>
  static
  auto TEA_CALL setter(unwrap_type obj, value_type value) -> void {
    return field::template setter<member>
      (std::forward<unwrap_type>(obj), std::forward<value_type>(value));
  }
};

template<typename member_type>
struct method_type;

template<typename class_type_t, typename return_type_t, typename ...args_type_t>
struct method_type<return_type_t (class_type_t:: *)(args_type_t...)> {
  using class_type = class_type_t;
  using return_type = return_type_t;
  using member_type = return_type (class_type:: *)(args_type_t...);
  using ref_type = class_type &;
  using obj_type = unwrap_pair_t<ref_type>;
  template<member_type member>
  static
  auto TEA_CALL invoke(obj_type obj, args_type_t ...args) -> return_type {
    return Class<class_type>::template Method<return_type, args_type_t...>::template invoke<member>
      (std::forward<obj_type>(obj), std::forward<args_type_t>(args)...);
  }
};

template<typename class_type_t, typename return_type_t, typename ...args_type_t>
struct method_type<return_type_t (class_type_t:: *)(args_type_t...) const> {
  using class_type = class_type_t;
  using return_type = return_type_t;
  using member_type = return_type (class_type:: *)(args_type_t...) const;
  using ref_type = const class_type &;
  using obj_type = unwrap_pair_t<ref_type>;
  template<member_type member>
  static
  auto TEA_CALL invoke(obj_type obj, args_type_t ...args) -> return_type {
    return Class<class_type>::template Method<return_type, args_type_t...>::template invoke<member>
      (std::forward<obj_type>(obj), std::forward<args_type_t>(args)...);
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
           typename = typename std::enable_if_t<is_args_match_v<member_type, std::tuple<args_type...>>>,
           typename method = method_type<member_type>,
           typename return_type = typename method::return_type,
           typename ref_type = typename method::ref_type,
           typename class_type = std::remove_reference_t<ref_type>,
           typename obj_type = typename method::obj_type>
  static
  auto TEA_CALL invoke(obj_type obj, args_type ...args) -> return_type {
    return method::template invoke<member>
      (std::forward<obj_type>(obj), std::forward<args_type>(args)...);
  }
};

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_REF_H_
