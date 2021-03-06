#ifndef LIB_TEAPOT_CORE_TIMER_H_
#define LIB_TEAPOT_CORE_TIMER_H_

#include <milktea.h>

#include <teapot/core/action.h>

#ifdef __cplusplus
extern "C" {
#endif

struct TeaPot_TimerFuture_t;

#ifndef __cplusplus
typedef struct TeaPot_TimerFuture_t TeaPot_TimerFuture_t;
#endif // ifndef __cplusplus

enum TeaPot_TimerFuture_State_t {
  TeaPot_TimerFuture_State_SCHEDULED,
  TeaPot_TimerFuture_State_EXECUTED,
  TeaPot_TimerFuture_State_CANCELLED,
  TeaPot_TimerFuture_State_NORMAL,
  TeaPot_TimerFuture_State_EXCEPTIONAL
};

#ifndef __cplusplus
typedef enum TeaPot_TimerFuture_State_t TeaPot_TimerFuture_State_t;
#endif // ifndef __cplusplus

MilkTea_api
TeaPot_TimerFuture_Destroy(TeaPot_TimerFuture_t *obj);

MilkTea_api
TeaPot_TimerFuture_Cancel(TeaPot_TimerFuture_t *obj, bool *success);

MilkTea_api
TeaPot_TimerFuture_GetState(TeaPot_TimerFuture_t *obj, TeaPot_TimerFuture_State_t *state);

MilkTea_api
TeaPot_TimerFuture_GetTime(TeaPot_TimerFuture_t *obj, int64_t *time);

struct TeaPot_TimerTask_t;

#ifndef __cplusplus
typedef struct TeaPot_TimerTask_t TeaPot_TimerTask_t;
#endif // ifndef __cplusplus

MilkTea_api
TeaPot_TimerTask_Destroy(TeaPot_TimerTask_t *obj);

MilkTea_api
TeaPot_TimerTask_GetFuture(TeaPot_TimerTask_t *obj, TeaPot_TimerFuture_t **future);

MilkTea_api
TeaPot_TimerTask_Run(TeaPot_TimerTask_t *obj);

MilkTea_Consumer_t(TeaPot_TimerTask_Consumer_t, TeaPot_TimerTask_t *)

#ifndef __cplusplus
typedef struct TeaPot_TimerTask_Consumer_t TeaPot_TimerTask_Consumer_t;
#endif

struct TeaPot_TimerWorker_t;

#ifndef __cplusplus
typedef struct TeaPot_TimerWorker_t TeaPot_TimerWorker_t;
#endif // ifndef __cplusplus

enum TeaPot_TimerWorker_State_t {
  TeaPot_TimerWorker_State_INIT,
  TeaPot_TimerWorker_State_RUNNING,
  TeaPot_TimerWorker_State_SHUTDOWN,
  TeaPot_TimerWorker_State_STOP,
  TeaPot_TimerWorker_State_TIDYING,
  TeaPot_TimerWorker_State_TERMINATED,
  TeaPot_TimerWorker_State_CLOSED
};

#ifndef __cplusplus
typedef enum TeaPot_TimerWorker_State_t TeaPot_TimerWorker_State_t;
#endif // ifndef __cplusplus

MilkTea_Closure_t(TeaPot_TimerWorker_Termination, bool, MilkTea_Exception_t, const char *)

#ifndef __cplusplus
typedef struct TeaPot_TimerWorker_Termination TeaPot_TimerWorker_Termination;
#endif // ifndef __cplusplus


MilkTea_api
TeaPot_TimerWorker_Create(TeaPot_TimerWorker_t **obj, TeaPot_TimerWorker_Termination termination);

MilkTea_api
TeaPot_TimerWorker_Destroy(TeaPot_TimerWorker_t *obj);

MilkTea_api
TeaPot_TimerWorker_Close(TeaPot_TimerWorker_t *obj);

MilkTea_api
TeaPot_TimerWorker_Start(TeaPot_TimerWorker_t *obj);

MilkTea_api
TeaPot_TimerWorker_GetState(TeaPot_TimerWorker_t *obj, TeaPot_TimerWorker_State_t *state);

MilkTea_api
TeaPot_TimerWorker_Post(TeaPot_TimerWorker_t *obj, TeaPot_TimerFuture_t **future, int64_t delay, TeaPot_Action_t action);

MilkTea_api
TeaPot_TimerWorker_Shutdown(TeaPot_TimerWorker_t *obj, bool *success);

MilkTea_api
TeaPot_TimerWorker_ShutdownNow(TeaPot_TimerWorker_t *obj, TeaPot_TimerTask_Consumer_t consumer);

MilkTea_api
TeaPot_TimerWorker_AwaitTermination(TeaPot_TimerWorker_t *obj, int64_t delay, bool *success);

struct TeaPot_TimerWorker_Weak_t;

#ifndef __cplusplus
typedef struct TeaPot_TimerWorker_Weak_t TeaPot_TimerWorker_Weak_t;
#endif // ifndef __cplusplus

MilkTea_api
TeaPot_TimerWorker_Weak_Create(TeaPot_TimerWorker_Weak_t **obj, TeaPot_TimerWorker_t *target);

MilkTea_api
TeaPot_TimerWorker_Weak_Destroy(TeaPot_TimerWorker_Weak_t *obj);

MilkTea_api
TeaPot_TimerWorker_Weak_Try(TeaPot_TimerWorker_Weak_t *obj, TeaPot_TimerWorker_t **target);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_TEAPOT_CORE_TIMER_H_
