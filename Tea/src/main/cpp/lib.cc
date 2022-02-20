#include "err.h"

using namespace tea;
using namespace tea::meta;

Err *&Err::cell() {
  thread_local Err *e = nullptr;
  return e;
}

extern "C" {

TEA_FUNC_LINK(tea_err_emit, Err::emit);
TEA_FUNC_LINK(tea_err_type, field_handle::getter<&Err::type_>);
TEA_FUNC_LINK(tea_err_what, method_handle<>::invoke<&Err::what>);
TEA_FUNC_LINK(tea_err_cause, method_handle<>::invoke<&Err::cause>);
TEA_FUNC_LINK(tea_err_suppressed, method_handle<>::invoke<&Err::suppressed>);
TEA_FUNC_LINK(tea_err_is, method_handle<tea_err_type_t>::invoke<&Err::is>);
TEA_FUNC_LINK(tea_err_dump, method_handle<tea_err_dump_recv_t>::invoke<&Err::dump>);

} // extern "C"

