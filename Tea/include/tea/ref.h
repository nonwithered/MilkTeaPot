#ifndef LIB_TEA_REF_H_
#define LIB_TEA_REF_H_

#ifdef __cplusplus

#include <memory>

#include <tea/def.h>
#include <tea/facade.h>

namespace tea {

namespace meta {

template<typename self_type>
struct Class final : empty_class {
  using cast_type = typename self_type::cast_type;
  using const_cast_type = typename self_type::const_cast_type;
  template<typename ...args_type>
  static auto TEA_CALL create(args_type ...args) -> cast_type {
    auto &ref = *std::make_unique<self_type>(std::forward<args_type>(args)...).release();
    return self_type::type_cast(ref);
  }
  static auto TEA_CALL destroy(cast_type obj) -> void {
    auto &ref = self_type::type_cast(std::forward<cast_type>(obj));
    drop(std::move(ref));
  }
  template<typename value_type>
  struct Field final : empty_class {
    template<value_type self_type:: *member>
    static auto TEA_CALL get(const_cast_type obj) -> value_type & {
      const auto &ref = self_type::type_cast(std::forward<const_cast_type>(obj));
      return ref.*member;
    }
    template<value_type self_type:: *member>
    static auto TEA_CALL set(cast_type obj, value_type value) -> void {
      auto &ref = self_type::type_cast(std::forward<cast_type>(obj));
      ref.*member = std::forward(value);
    }
  };
  template<typename result_type, typename ...args_type>
  struct Method final : empty_class {
    template<result_type (self_type:: *member)(args_type...) const>
    static auto TEA_CALL invoke(const_cast_type obj, args_type ...args) -> result_type {
      const auto &ref = self_type::type_cast(std::forward<const_cast_type>(obj));
      return (ref.*member)(std::forward<args_type>(args)...);
    }
    template<result_type (self_type:: *member)(args_type...)>
    static auto TEA_CALL invoke(cast_type obj, args_type ...args) -> result_type {
      auto &ref = self_type::type_cast(std::forward<cast_type>(obj));
      return (ref.*member)(std::forward<args_type>(args)...);
    }
  };
};

} // namespace meta

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_REF_H_
