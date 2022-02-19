#include <tea/err.h>

namespace tea {

struct Err final : remove_copy<Err>, remove_move<Err> {
  const err_type_t type_;
  Err *const cause_;
  const std::string what_;
  Err *const suppressed_;
};

} // namespace tea
