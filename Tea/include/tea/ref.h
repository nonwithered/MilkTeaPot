#ifndef LIB_TEA_REF_H_
#define LIB_TEA_REF_H_

#ifdef __cplusplus

#include <memory>

#include <tea/def.h>
#include <tea/facade.h>

namespace tea {

template<typename self_type>
struct Class final : empty_class {
  using raw_type = typename self_type::raw_type;
  template<typename ...args_type>
  static auto TEA_CALL create(args_type ...args) -> raw_type {
    auto &ref = *std::make_unique<self_type>(std::forward<args_type>(args)...).release();
    return type_cast<self_type &, raw_type>(ref);
  }
  static auto TEA_CALL destroy(raw_type obj) -> void {
    auto &ref = type_cast<raw_type, self_type &>(std::forward<raw_type>(obj));
    drop(std::move(ref));
  }
  template<typename value_type>
  struct Field final : empty_class {
    template<value_type self_type:: *member>
    static auto TEA_CALL get(const raw_type obj) -> value_type {
      const auto &ref = type_cast<const raw_type, const self_type &>(std::forward<const raw_type>(obj));
      return ref.*member;
    }
    template<value_type self_type:: *member>
    static auto TEA_CALL set(raw_type obj, value_type value) -> void {
      auto &ref = type_cast<raw_type, self_type &>(std::forward<raw_type>(obj));
      ref.*member = std::forward(value);
    }
  };
  template<typename result_type, typename ...args_type>
  struct Method final : empty_class {
    template<result_type (self_type:: *member)(args_type...) const>
    static auto TEA_CALL invoke(const raw_type obj, args_type ...args) -> result_type {
      const auto &ref = type_cast<const raw_type, const self_type &>(std::forward<const raw_type>(obj));
      return (ref.*member)(std::forward<args_type>(args)...);
    }
    template<result_type (self_type:: *member)(args_type...)>
    static auto TEA_CALL invoke(raw_type obj, args_type ...args) -> result_type {
      auto &ref = type_cast<raw_type, self_type &>(std::forward<raw_type>(obj));
      return (ref.*member)(std::forward<args_type>(args)...);
    }
  };
};

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_REF_H_
