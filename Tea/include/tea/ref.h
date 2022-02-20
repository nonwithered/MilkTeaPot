#ifndef LIB_TEA_REF_H_
#define LIB_TEA_REF_H_

#ifdef __cplusplus

#include <memory>

#include <tea/def.h>
#include <tea/facade.h>
#include <tea/cast.h>

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
    template<value_type class_type:: *member>
    static
    auto TEA_CALL get(const_unwrap_type obj) -> value_type & {
      const auto &ref = wrap_cast(std::forward<const_unwrap_type>(obj));
      return ref.*member;
    }
    template<value_type class_type:: *member>
    static
    auto TEA_CALL set(unwrap_type obj, value_type value) -> void {
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

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_REF_H_
