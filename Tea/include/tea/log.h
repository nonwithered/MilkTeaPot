#ifndef LIB_TEA_LOG_H_
#define LIB_TEA_LOG_H_

#ifdef __cplusplus
#include <functional>
#include <string>
#include <type_traits>
#include <string>
#include <cstdio>
#include <tea/ref.h>
#endif // ifdef __cplusplus

#include <tea/def.h>

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

enum tea_log_level_t {
  D, I, W, E, A
};

extern
TEA_API void (TEA_CALL *
tea_log_print)(tea_log_level_t priority, const char tag[], const char msg[]);

extern
TEA_API tea_log_level_t (TEA_CALL *
tea_log_priority)();

struct tea_logger_t {
  struct tea_log_ctx_t *ctx;
  void (*print)(struct tea_log_ctx_t *, enum tea_log_level_t, const char [], const char []);
  enum tea_log_level_t (*priority)(struct tea_log_ctx_t *);
  void (*close)(struct tea_log_ctx_t *);
};

extern TEA_API void (TEA_CALL *
tea_logger_setup)(const tea_logger_t *);

#ifdef __cplusplus
} // extern "C"
#endif // ifdef __cplusplus

#ifdef __cplusplus

namespace tea {

using log_level = tea_log_level_t;
using log_ctx = tea_log_ctx_t;

template<typename T, std::size_t N = 1024,
         typename class_type = T,
         typename = std::enable_if_t<std::is_class_v<class_type>>>
class with_log {
  static
  auto TAG() -> const char * {
    return T::TAG;
  }
 protected:
  with_log() = default;
  static constexpr auto D = log_level::D;
  static constexpr auto I = log_level::I;
  static constexpr auto W = log_level::W;
  static constexpr auto E = log_level::E;
  template<log_level priority, typename ...args_type>
  static
  auto log(const char fmt[], args_type ...args) -> void {
    if (priority < tea_log_priority()) {
      return;
    }
    std::string msg(N, '\0');
    std::snprintf(msg.data(), N, fmt, std::forward<args_type>(args)...);
    tea_log_print(priority, TAG(), msg.data());
  }
  template<log_level priority>
  static
  auto log(std::function<std::string()> func) -> void {
    if (priority < tea_log_priority()) {
      return;
    }
    auto msg = func();
    tea_log_print(priority, TAG(), msg.data());
  }
};

namespace logger {

template<typename T,
         typename class_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         void (class_type:: *print)(log_level, const char *, const char *) = &class_type::Print,
         log_level (class_type:: *priority)() = &class_type::Priority>
auto setup(T &&ref) -> void {
  auto logger = tea_logger_t {
    .ctx = reinterpret_cast<tea::log_ctx *>(&ref),
    .print = tea::meta::method_handle<log_level, const char *, const char *>::invoke<print>,
    .priority = tea::meta::method_handle<>::invoke<priority>,
    .close = tea::meta::Class<class_type>::destroy,
  };
  tea_logger_setup(&logger);
}

} // namespace logger
} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_LOG_H_
