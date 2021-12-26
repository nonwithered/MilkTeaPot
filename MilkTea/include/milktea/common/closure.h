#ifndef LIB_MILKTEA_COMMON_CLOSURE_H_
#define LIB_MILKTEA_COMMON_CLOSURE_H_

#include <functional>
#include <memory>

#include <milktea/core.h>

namespace MilkTea {

template<typename Res, typename... Args>
class FunctionFactory {
 public:
  static Res MilkTea_call Invoke(void *self, Args... args) {
    std::function<Res(Args...)> &callback_ = *reinterpret_cast<std::function<Res(Args...)> *>(self);
    return callback_(args...);
  }
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
      return f.invoke_(f.self_, args...);
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
  static MilkTea_ClosureToken_t ToRawType(const std::function<Res(Args...)> &f) {
    return MilkTea_ClosureToken_t{
      .self_ = new std::function<Res(Args...)>(f),
      .deleter_ = Deteler,
    };
  }
  static std::function<Res(Args...)> FromRawType(MilkTea_ClosureToken_t token, std::function<Res(void *, Args...)> invoke) {
    MilkTea_nonnull(token.deleter_);
    auto closure = std::shared_ptr<void>(token.self_, token.deleter_);
    return [closure, invoke](Args... args) -> Res {
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
