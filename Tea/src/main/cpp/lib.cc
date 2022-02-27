#include "err.h"
#include "log.h"

auto tea::internal::errors::Err::cell() -> Err *& {
  thread_local Err *e = nullptr;
  return e;
}

auto tea::internal::log::cell() -> logger & {
  static logger obj{};
  static tea::defer sweep = []() {
    auto func = obj.close;
    if (func == nullptr) {
      return;
    }
    func(obj.ctx);
  };
  return obj;
}

namespace {

using namespace tea::meta;

using namespace tea::internal;

using errors::Err;

auto err_emit = function_handle<tea_err_type_t, const char *, tea_err_t *>::invoke<&Err::emit>;
auto err_type_of = field_handle::getter<&Err::type_>;
auto err_what = method_handle<>::invoke<&Err::what>;
auto err_cause = method_handle<>::invoke<&Err::cause>;
auto err_suppressed = method_handle<>::invoke<&Err::suppressed>;
auto err_is = method_handle<tea_err_type_t>::invoke<&Err::is>;
auto err_dump = method_handle<tea_err_dump_recv_t>::invoke<&Err::dump>;

auto log_print = function_handle<tea::log_level, const char *, const char *>::invoke<&log::print>;
auto log_priority = function_handle<>::invoke<&log::priority>;
auto logger_setup = function_handle<const log::logger *>::invoke<&log::setup>;

} // namespace

extern "C" {

TEA_FUNC(tea, err_emit);
TEA_FUNC(tea, err_type_of);
TEA_FUNC(tea, err_what);
TEA_FUNC(tea, err_cause);
TEA_FUNC(tea, err_suppressed);
TEA_FUNC(tea, err_is);
TEA_FUNC(tea, err_dump);

TEA_FUNC(tea, log_print);
TEA_FUNC(tea, log_priority);
TEA_FUNC(tea, logger_setup);

TEA_ERR_ENUM_IMPL(tea_err_type_bad_logic, nullptr)
TEA_ERR_ENUM_IMPL(tea_err_type_invalid_param, tea_err_type_bad_logic)
TEA_ERR_ENUM_IMPL(tea_err_type_null_obj, tea_err_type_invalid_param)
TEA_ERR_ENUM_IMPL(tea_err_type_out_of_range, tea_err_type_invalid_param)
TEA_ERR_ENUM_IMPL(tea_err_type_cast_failure, tea_err_type_invalid_param)
TEA_ERR_ENUM_IMPL(tea_err_type_invalid_state, tea_err_type_bad_logic)
TEA_ERR_ENUM_IMPL(tea_err_type_assertion, tea_err_type_invalid_state)
TEA_ERR_ENUM_IMPL(tea_err_type_runtime_warn, nullptr)
TEA_ERR_ENUM_IMPL(tea_err_type_unsupported, tea_err_type_runtime_warn)
TEA_ERR_ENUM_IMPL(tea_err_type_io_failure, tea_err_type_runtime_warn)
TEA_ERR_ENUM_IMPL(tea_err_type_unexpected_eof, tea_err_type_io_failure)
TEA_ERR_ENUM_IMPL(tea_err_type_undefined_format, tea_err_type_io_failure)

extern TEA_API void (TEA_CALL *
tea_logger_apply)(const tea_logger_t *);

} // extern "C"

