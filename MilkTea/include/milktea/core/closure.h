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

#define MilkTea_Function_t(S, R, ...) \
struct S { \
  void *self_; \
  R (MilkTea_call *invoke_)(void *, ##__VA_ARGS__); \
};

#define MilkTea_Consumer_t(S, T) MilkTea_Function_t(S, void, T)

#define MilkTea_Supplier_t(S, T) MilkTea_Function_t(S, T)

#endif // ifndef LIB_MILKTEA_CORE_CLOSURE_H_
