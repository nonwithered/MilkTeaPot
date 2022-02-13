#ifndef LIB_MILKTEA_WRAPPER_EXCEPT_H_
#define LIB_MILKTEA_WRAPPER_EXCEPT_H_

#define MilkTea_with_except(block) { \
  return MilkTea::Exception::ToRawType(MilkTea::Exception::Suppress([&]() block)); \
}

#define MilkTea_extern(section, list, block) \
extern "C" \
MilkTea_Exception_t \
section list { \
  MilkTea_with_except(block) \
}

#include <functional>

#include <milktea/common.h>

namespace MilkTea {
namespace Exception {

inline
std::string_view What() {
  return MilkTea_Exception_What();
}

MilkTea_noreturn
inline
void Raise(Type type, std::string_view what) {
  MilkTea_Exception_Raise(ToRawType(type), what.data());
}

inline
Type Suppress(const std::function<void()> &block) {
  return FromRawType(MilkTea_Exception_Suppress(FunctionFactory<void()>::ToRawType<MilkTea_Exception_Block_t>(block)));
}

} // namespace Exception
} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_WRAPPER_EXCEPT_H_
