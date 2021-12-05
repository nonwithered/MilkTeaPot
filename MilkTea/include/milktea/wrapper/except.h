#ifndef LIB_MILKTEA_WRAPPER_EXCEPT_H_
#define LIB_MILKTEA_WRAPPER_EXCEPT_H_

#include <milktea/common.h>
namespace MilkTea {
namespace Exception {

inline
std::string_view What() {
  return MilkTea_Exception_What();
}

MilkTea_noreturn
inline
void Throw(Type type, std::string_view what) {
  MilkTea_Exception_Throw(ToRawType(type), what.data());
}

MilkTea_decl(Type)
Unwrap(std::exception *e);

} // namespace Exception
} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_WRAPPER_EXCEPT_H_
