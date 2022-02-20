#ifndef TEA_LOG_H_
#define TEA_LOG_H_

#include <tea.h>

namespace tea {

using logger = tea_logger_t;

auto logger_cell() -> logger &;

auto TEA_CALL log_print(log_level priority, const char tag[], const char msg[]) -> void {
  auto &obj = logger_cell();
  auto func = obj.print;
  if (func == nullptr) {
    return;
  }
  auto ctx = obj.ctx;
  func(ctx, priority, tag, msg);
}

auto TEA_CALL log_priority() -> log_level {
  auto &obj = logger_cell();
  auto func = obj.priority;
  if (func == nullptr) {
    return log_level::A;
  }
  auto ctx = obj.ctx;
  return func(ctx);
}

auto TEA_CALL logger_apply(const logger *ptr) -> void {
  if (ptr == nullptr) {
    err::raise<err_enum::null_obj>("logger_apply: obj");
    return;
  }
  if (ptr->priority == nullptr) {
    err::raise<err_enum::null_obj>("logger_apply: priority");
    return;
  }
  if (ptr->print == nullptr) {
    err::raise<err_enum::null_obj>("logger_apply: print");
    return;
  }
  logger_cell() = *ptr;
}

} // namespace tea

#endif // ifndef TEA_LOG_H_
