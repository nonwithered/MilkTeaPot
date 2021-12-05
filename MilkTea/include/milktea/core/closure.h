#ifndef LIB_MILKTEA_CORE_CLOSURE_H_
#define LIB_MILKTEA_CORE_CLOSURE_H_

#include <milktea/core.h>

struct MilkTea_ClosureToken_t {
  void *self_;
  void (MilkTea_call *deleter_)(void *);
};

#ifndef __cplusplus
typedef struct MilkTea_ClosureToken_t MilkTea_ClosureToken_t;
#endif // ifndef __cplusplus

#endif // ifndef LIB_MILKTEA_CORE_CLOSURE_H_
