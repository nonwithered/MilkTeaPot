#ifndef LIB_MILKTEA_COMMON_CLOSURE_H_
#define LIB_MILKTEA_COMMON_CLOSURE_H_

#include <functional>
#include <memory>

#include <milktea/core.h>

namespace MilkTea {

template<typename Res, typename... Args>
class ClosureFactory;

template<typename Res, typename... Args>
class FunctionFactory {
  friend class ClosureFactory<Res, Args...>;
  static Res MilkTea_call Invoke(void *self, Args... args) {
    std::function<Res(Args...)> &callback_ = *reinterpret_cast<std::function<Res(Args...)> *>(self);
    return callback_(args...);
  }
 public:
  template<typename raw_type>
  static raw_type ToRawType(const std::function<Res(Args...)> &f) {
    return raw_type{
      .self_ = const_cast<std::function<Res(Args...)> *>(&f),
      .invoke_ = FunctionFactory<Res(Args...)>::Invoke,
    };
  }
  template<typename raw_type>
  static std::function<Res(Args...)> FromRawType(raw_type f) {
    MilkTea_nonnull(f.invoke_);
    return [f](Args... args) -> Res {
      return MilkTea_Function_Invoke(f, args...);
    };
  }
};

template<typename Res, typename... Args>
class FunctionFactory<Res(Args...)> : public FunctionFactory<Res, Args...> {};

template<typename Res, typename... Args>
class FunctionFactory<std::function<Res(Args...)>> : public FunctionFactory<Res(Args...)> {};

template<typename Res, typename... Args>
class ClosureFactory {
 public:
  template<typename raw_type>
  static raw_type ToRawType(const std::function<Res(Args...)> &f) {
    return raw_type{
      .self_ = MilkTea_ClosureToken_t{
        .self_ = new std::function<Res(Args...)>(f),
        .deleter_ = Deteler,
      },
      .invoke_ = FunctionFactory<Res(Args...)>::Invoke,
    };
  }
  template<typename raw_type>
  static std::function<Res(Args...)> FromRawType(raw_type f) {
    MilkTea_nonnull(f.self_.deleter_);
    auto closure = std::shared_ptr<void>(f.self_.self_, f.self_.deleter_);
    MilkTea_nonnull(f.invoke_);
    return [closure, invoke = f.invoke_](Args... args) -> Res {
      return invoke(closure.get(), args...);
    };
  }
 private:
  static void MilkTea_call Deteler(void *self) {
    delete reinterpret_cast<std::function<Res(Args...)> *>(self);
  }
  static constexpr char TAG[] = "MilkTea#ClosureFactory";
};

template<typename Res, typename... Args>
class ClosureFactory<Res(Args...)> : public ClosureFactory<Res, Args...> {};

template<typename Res, typename... Args>
class ClosureFactory<std::function<Res(Args...)>> : public ClosureFactory<Res(Args...)> {};

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_CLOSURE_H_
