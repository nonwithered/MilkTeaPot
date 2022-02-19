#include "err.h"

using namespace tea;
using namespace tea::meta;

Err *&Err::cell() {
  thread_local Err *e = nullptr;
  return e;
}

extern "C" {

auto tea_err_emit(err_type_t type, const char what[], err_t *cause) -> err_t * {
  return Err::emit(type, what, cause);
}

auto tea_err_type(const err_t *obj) -> err_type_t {
  return Class<Err>::Field<const err_type_t>::get<&Err::type_>(obj);
}

auto tea_err_what(const err_t *obj) -> const char * {
  return Class<Err>::Method<const char *>::invoke<&Err::what>(obj);
}

auto tea_err_cause(const err_t *obj) -> const err_t * {
  return Class<Err>::Method<const err_t *>::invoke<&Err::cause>(obj);
}

auto tea_err_suppressed(const err_t *obj) -> const err_t * {
  return unwrap_cast(*Class<Err>::Field<Err *const>::get<&Err::suppressed_>(obj));
}

auto tea_err_is(const tea_err_t *obj, tea_err_type_t type) -> bool {
  return Class<Err>::Method<bool, err_type_t>::invoke<&Err::is>(obj, type);
}

auto tea_err_dump(const err_t *obj, tea_err_dump_recv_t recv) -> void {
  Class<Err>::Method<void, tea_err_dump_recv_t>::invoke<&Err::dump>(obj, recv);
}

} // extern "C"

