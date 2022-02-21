#ifndef TEA_LOG_H_
#define TEA_LOG_H_

#include <tea.h>

namespace tea {
namespace internal {
namespace log {

using logger = tea_logger_t;
using level = log_level;

auto cell() -> logger &;

auto print(level priority, const char tag[], const char msg[]) -> void {
  auto &obj = cell();
  auto func = obj.print;
  if (func == nullptr) {
    return;
  }
  auto ctx = obj.ctx;
  func(ctx, priority, tag, msg);
}

auto priority() -> level {
  auto &obj = cell();
  auto func = obj.priority;
  if (func == nullptr) {
    return level::A;
  }
  auto ctx = obj.ctx;
  return func(ctx);
}

auto setup(const logger *ptr) -> void {
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
  cell() = *ptr;
}

} // namespace log
} // namespace internal
} // namespace tea

#endif // ifndef TEA_LOG_H_
