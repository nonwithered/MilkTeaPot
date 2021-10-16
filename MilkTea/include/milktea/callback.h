#ifndef LIB_MILKTEA_CALLBACK_H_
#define LIB_MILKTEA_CALLBACK_H_

#ifdef __cplusplus
#include <milktea/util.h>
#include <functional>
namespace MilkTea {

template<typename Res, typename... Args>
struct CallbackToken {
  CallbackToken(const std::function<Res(Args...)> *callback = nullptr) {}
  static Res MilkTea_CALL Invoke(void *obj, Args... args) {
    std::function<Res(Args...)> &callback_ = *reinterpret_cast<std::function<Res(Args...)> *>(obj);
    return callback_(args...);
  }
};

template<typename Res, typename... Args>
struct CallbackToken<Res(Args...)> : public CallbackToken<Res, Args...> {};

template<typename Res, typename... Args>
struct CallbackToken<std::function<Res(Args...)>> : public CallbackToken<Res(Args...)> {};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_CALLBACK_H_
