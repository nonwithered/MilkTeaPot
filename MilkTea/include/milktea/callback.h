#ifndef LIB_MILKTEA_CALLBACK_H_
#define LIB_MILKTEA_CALLBACK_H_

#include <milktea/exception.h>

struct MilkTea_ClosureToken_t {
  void *self_;
  void (MilkTea_CALL *deleter_)(void *);
};

#ifndef __cplusplus
typedef struct MilkTea_ClosureToken_t MilkTea_ClosureToken_t;
#endif // ifndef __cplusplus

#ifdef __cplusplus
#include <milktea/util.h>
#include <functional>
#include <memory>
namespace MilkTea {

template<typename Res, typename... Args>
class ClosureToken {
 public:
  static Res MilkTea_CALL Invoke(void *self, Args... args) {
    std::function<Res(Args...)> &callback_ = *reinterpret_cast<std::function<Res(Args...)> *>(self);
    return callback_(args...);
  }
  static MilkTea_ClosureToken_t ToRawType(const std::function<Res(Args...)> &f) {
    return MilkTea_ClosureToken_t{
      .self_ = new std::function<Res(Args...)>(f),
      .deleter_ = Deteler,
    };
  }
  static std::function<Res(Args...)> FromRawType(MilkTea_ClosureToken_t token, std::function<Res(void *, Args...)> invoke) {
    auto deleter = token.deleter_;
    MilkTea_nonnull(deleter);
    auto closure = std::shared_ptr<void>(token.self_, deleter);
    return [closure, invoke](Args... args) -> Res {
      return invoke(closure.get(), args...);
    };
  }
 private:
  static void MilkTea_CALL Deteler(void *self) {
    delete reinterpret_cast<std::function<Res(Args...)> *>(self);
  }
  static constexpr char TAG[] = "MilkTea#ClosureToken";
};

template<typename Res, typename... Args>
class ClosureToken<Res(Args...)> : public ClosureToken<Res, Args...> {};

template<typename Res, typename... Args>
class ClosureToken<std::function<Res(Args...)>> : public ClosureToken<Res(Args...)> {};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_CALLBACK_H_
