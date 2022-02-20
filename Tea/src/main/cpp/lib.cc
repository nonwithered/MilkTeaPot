#include "err.h"
#include "log.h"

auto tea::Err::cell() -> Err *& {
  thread_local Err *e = nullptr;
  return e;
}

auto tea::logger_cell() -> logger & {
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

using namespace tea;
using namespace tea::meta;

extern "C" {

TEA_FUNC_LINK(tea_err_emit, Err::emit);
TEA_FUNC_LINK(tea_err_type, field_handle::getter<&Err::type_>);
TEA_FUNC_LINK(tea_err_what, method_handle<>::invoke<&Err::what>);
TEA_FUNC_LINK(tea_err_cause, method_handle<>::invoke<&Err::cause>);
TEA_FUNC_LINK(tea_err_suppressed, method_handle<>::invoke<&Err::suppressed>);
TEA_FUNC_LINK(tea_err_is, method_handle<tea_err_type_t>::invoke<&Err::is>);
TEA_FUNC_LINK(tea_err_dump, method_handle<tea_err_dump_recv_t>::invoke<&Err::dump>);

TEA_FUNC_LINK(tea_log_print, log_print);
TEA_FUNC_LINK(tea_log_priority, log_priority);
TEA_FUNC_LINK(tea_logger_setup, logger_apply);

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

