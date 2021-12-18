#ifndef LIB_TEAPOT_CORE_ACTION_H_
#define LIB_TEAPOT_CORE_ACTION_H_

#include <milktea.h>

struct TeaPot_Action_t {
  MilkTea_ClosureToken_t self_;
  void (MilkTea_call *run_)(void *self);
};

#ifndef __cplusplus
typedef struct TeaPot_Action_t TeaPot_Action_t;
#endif // ifndef __cplusplus

struct TeaPot_Executor_t {
  MilkTea_ClosureToken_t self_;
  void (MilkTea_call *submit_)(void *self, TeaPot_Action_t action);
};

#ifndef __cplusplus
typedef struct TeaPot_Executor_t TeaPot_Executor_t;
#endif // ifndef __cplusplus

#endif // ifndef LIB_TEAPOT_CORE_ACTION_H_
