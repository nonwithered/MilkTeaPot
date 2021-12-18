#ifndef LIB_MILKTEA_COMMON_MACRO_H_
#define LIB_MILKTEA_COMMON_MACRO_H_

#include <milktea/core.h>

#define MilkTea_NonCopy(T) \
  T(const T &) = delete; \
  void operator=(const T &) = delete;
#define MilkTea_NonMove(T) \
  T(T &&) = delete; \
  void operator=(T &&) = delete;
#define MilkTea_NonCopyAssign(T) \
  void operator=(const T &) = delete;
#define MilkTea_NonMoveAssign(T) \
  void operator=(T &&) = delete;
#define MilkTea_NonAssign(T) \
  MilkTea_NonCopyAssign(T) \
  MilkTea_NonMoveAssign(T)

#endif // ifndef LIB_MILKTEA_COMMON_MACRO_H_