#ifndef LIB_MILKTEA_CALLBACK_H_
#define LIB_MILKTEA_CALLBACK_H_

#ifdef __cplusplus
#include <milktea/util.h>
#include <functional>
namespace MilkTea {

template<typename Res, typename... Args>
struct CallbackOf {
  CallbackOf(const std::function<Res(Args...)> *callback = nullptr) {}
  static Res MilkTea_CALL Token(void *obj, Args... args) {
    std::function<Res(Args...)> &callback_ = *reinterpret_cast<std::function<Res(Args...)> *>(obj);
    return callback_(args...);
  }
};

template<typename Res, typename... Args>
struct CallbackOf<Res(Args...)> : public CallbackOf<Res, Args...> {};

template<typename Res, typename... Args>
struct CallbackOf<std::function<Res(Args...)>> : public CallbackOf<Res(Args...)> {};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_CALLBACK_H_
