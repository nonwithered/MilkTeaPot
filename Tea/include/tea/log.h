#ifndef LIB_TEA_LOG_H_
#define LIB_TEA_LOG_H_

#ifdef __cplusplus
#include <functional>
#include <string>
#include <type_traits>
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
  enum tea_log_level_t (*priority)(const struct tea_log_ctx_t *);
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

template<typename T,
         typename class_type = T,
         typename = std::enable_if_t<std::is_class_v<class_type>>>
class Log {
 protected:
  Log() = default;
  static constexpr auto D = log_level::D;
  static constexpr auto I = log_level::I;
  static constexpr auto W = log_level::W;
  static constexpr auto E = log_level::E;
  template<log_level priority>
  static
  auto log(const char msg[]) -> void {
    if (priority < tea_log_priority()) {
      return;
    }
    tea_log_print(priority, class_type::TAG(), msg);
  }
  template<log_level priority>
  static
  auto log(const std::function<std::string()> &func) {
    if (priority < tea_log_priority()) {
      return;
    }
    auto msg = func();
    tea_log_print(priority, class_type::TAG(), msg.data());
  }
};

template<typename T,
         typename class_type = std::remove_reference_t<T>,
         typename = std::enable_if_t<std::is_class_v<class_type>>,
         auto print_method = &class_type::Print,
         typename print_method_type = decltype(print_method),
         typename = std::enable_if_t<std::is_member_function_pointer_v<print_method_type>>,
         typename = std::enable_if_t<std::is_same_v<print_method_type, void (class_type:: *)(log_level, const char *, const char *)>>,
         auto priority_method = &class_type::Priority,
         typename priority_method_type = decltype(priority_method),
         typename = std::enable_if_t<std::is_member_function_pointer_v<priority_method_type>>,
         typename = std::enable_if_t<std::is_same_v<priority_method_type, log_level (class_type:: *)() const>>
         >
auto setup_logger(T &&ref) -> void {
  auto logger = tea_logger_t {
    .ctx = reinterpret_cast<tea::log_ctx *>(&ref),
    .print = tea::meta::method_handle<log_level, const char *, const char *>::invoke<print_method>,
    .priority = tea::meta::method_handle<>::invoke<priority_method>,
    .close = tea::meta::Class<class_type>::destroy,
  };
  tea_logger_setup(&logger);
}

} // namespace tea

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEA_LOG_H_
