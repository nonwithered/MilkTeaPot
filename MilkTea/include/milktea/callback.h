#ifndef LIB_MILKTEA_CALLBACK_H_
#define LIB_MILKTEA_CALLBACK_H_

#ifdef __cplusplus
#include <functional>
namespace MilkTea {

template<typename Res, typename... Args>
Res (*CallbackOf(const std::function<Res(Args...)> &))(void *, Args...) {
  return [](void *obj, Args... args) -> Res {
    std::function<Res(Args...)> &callback = *reinterpret_cast<std::function<Res(Args...)> *>(obj);
    return callback(args...);
  };
}

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_CALLBACK_H_
