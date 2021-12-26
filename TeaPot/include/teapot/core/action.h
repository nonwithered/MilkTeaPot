#ifndef LIB_TEAPOT_CORE_ACTION_H_
#define LIB_TEAPOT_CORE_ACTION_H_

#include <milktea.h>

MilkTea_Closure_t(TeaPot_Action_t, void)

#ifndef __cplusplus
typedef struct TeaPot_Action_t TeaPot_Action_t;
#endif // ifndef __cplusplus

MilkTea_Closure_t(TeaPot_Executor_t, void, TeaPot_Action_t)

#ifndef __cplusplus
typedef struct TeaPot_Executor_t TeaPot_Executor_t;
#endif // ifndef __cplusplus

#endif // ifndef LIB_TEAPOT_CORE_ACTION_H_
