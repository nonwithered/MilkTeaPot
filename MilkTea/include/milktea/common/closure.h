#ifndef LIB_MILKTEA_COMMON_CLOSURE_H_
#define LIB_MILKTEA_COMMON_CLOSURE_H_

#include <functional>
#include <memory>

#include <milktea/core.h>

namespace MilkTea {

template<typename Res, typename... Args>
class BaseClosureFactory;

template<typename Res, typename... Args>
class BaseFunctionFactory {
  static constexpr char TAG[] = "MilkTea#BaseFunctionFactory";
  friend class BaseClosureFactory<Res, Args...>;
 public:
  using function_type = std::function<Res(Args...)>;
 private:
  static Res MilkTea_call Invoke(void *self, Args... args) {
    function_type &callback_ = *reinterpret_cast<function_type *>(self);
    return callback_(args...);
  }
 public:
  template<typename raw_type>
  static raw_type ToRawType(const function_type &f) {
    return raw_type{
      .self_ = const_cast<function_type *>(&f),
      .invoke_ = BaseFunctionFactory<Res, Args...>::Invoke,
    };
  }
  template<typename raw_type>
  static function_type FromRawType(raw_type f) {
    MilkTea_nonnull(f.invoke_);
    return [f](Args... args) -> Res {
      return MilkTea_Function_Invoke(f, args...);
    };
  }
};

template<typename Res, typename... Args>
class BaseClosureFactory {
  static constexpr char TAG[] = "MilkTea#BaseClosureFactory";
 public:
  using function_type = std::function<Res(Args...)>;
 public:
  template<typename raw_type>
  static raw_type ToRawType(const function_type &f) {
    return raw_type{
      .self_ = MilkTea_ClosureToken_t{
        .self_ = new function_type(f),
        .deleter_ = Deteler,
      },
      .invoke_ = BaseFunctionFactory<Res, Args...>::Invoke,
    };
  }
  template<typename raw_type>
  static function_type FromRawType(raw_type f) {
    MilkTea_nonnull(f.self_.deleter_);
    auto closure = std::shared_ptr<void>(f.self_.self_, f.self_.deleter_);
    MilkTea_nonnull(f.invoke_);
    return [closure, invoke = f.invoke_](Args... args) -> Res {
      return invoke(closure.get(), args...);
    };
  }
 private:
  static void MilkTea_call Deteler(void *self) {
    delete reinterpret_cast<function_type *>(self);
  }
};

template<typename Res, typename... Args>
class FunctionFactory final : public BaseFunctionFactory<Res, Args...> {};

template<typename Res, typename... Args>
class FunctionFactory<Res(Args...)> final : public BaseFunctionFactory<Res, Args...> {};

template<typename Res, typename... Args>
class FunctionFactory<std::function<Res(Args...)>> final : public BaseFunctionFactory<Res(Args...)> {};

template<typename Res, typename... Args>
class ClosureFactory final : public BaseClosureFactory<Res, Args...> {};

template<typename Res, typename... Args>
class ClosureFactory<Res(Args...)> final : public BaseClosureFactory<Res, Args...> {};

template<typename Res, typename... Args>
class ClosureFactory<std::function<Res(Args...)>> final : public BaseClosureFactory<Res(Args...)> {};

#undef MilkTea_Function_t
#define MilkTea_Function_t(S, R, ...) \
struct S { \
  using factory_type = MilkTea::FunctionFactory<R(__VA_ARGS__)>; \
  void *self_; \
  R (MilkTea_call *invoke_)(void *, ##__VA_ARGS__); \
};

#undef MilkTea_Closure_t
#define MilkTea_Closure_t(S, R, ...) \
struct S { \
  using factory_type = MilkTea::ClosureFactory<R(__VA_ARGS__)>; \
  MilkTea_ClosureToken_t self_; \
  R (MilkTea_call *invoke_)(void *, ##__VA_ARGS__); \
};

namespace FunctionAdapter {

template<typename raw_type>
typename raw_type::factory_type::function_type FromRawType(raw_type f) {
  return raw_type::factory_type::template FromRawType<raw_type>(f);
}

template<typename raw_type>
raw_type ToRawType(const typename raw_type::factory_type::function_type &f) {
  return raw_type::factory_type::template ToRawType<raw_type>(f);
}

} // namespace FunctionAdapter

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_CLOSURE_H_
