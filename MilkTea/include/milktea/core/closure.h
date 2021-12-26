#ifndef LIB_MILKTEA_CORE_CLOSURE_H_
#define LIB_MILKTEA_CORE_CLOSURE_H_

#include <milktea/core.h>

#define MilkTea_Function_t(S, R, ...) \
struct S { \
  void *self_; \
  R (MilkTea_call *invoke_)(void *, ##__VA_ARGS__); \
};

#define MilkTea_Function_Invoke(it, ...) \
it.invoke_(it.self_, ##__VA_ARGS__)

#define MilkTea_Consumer_t(S, T) MilkTea_Function_t(S, void, T)

#define MilkTea_Supplier_t(S, T) MilkTea_Function_t(S, T)

struct MilkTea_ClosureToken_t {
  void *self_;
  void (MilkTea_call *deleter_)(void *);
};

#ifndef __cplusplus
typedef struct MilkTea_ClosureToken_t MilkTea_ClosureToken_t;
#endif // ifndef __cplusplus

#define MilkTea_Closure_t(S, R, ...) \
struct S { \
  struct MilkTea_ClosureToken_t self_; \
  R (MilkTea_call *invoke_)(void *, ##__VA_ARGS__); \
};

#define MilkTea_Closure_Invoke(it, ...) \
it.invoke_(it.self_.self_, ##__VA_ARGS__)

#endif // ifndef LIB_MILKTEA_CORE_CLOSURE_H_
