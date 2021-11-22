#ifndef LIB_MILKTEA_TIMER_H_
#define LIB_MILKTEA_TIMER_H_

#include <milktea/exception.h>
#include <milktea/callback.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MilkTea_TimerFuture_t;

#ifndef __cplusplus
typedef struct MilkTea_TimerFuture_t MilkTea_TimerFuture_t;
#endif // ifndef __cplusplus

enum MilkTea_TimerFuture_State_t {
  MilkTea_TimerFuture_State_SCHEDULED,
  MilkTea_TimerFuture_State_EXECUTED,
  MilkTea_TimerFuture_State_CANCELLED,
  MilkTea_TimerFuture_State_NORMAL,
  MilkTea_TimerFuture_State_EXCEPTIONAL
};

#ifndef __cplusplus
typedef enum MilkTea_TimerFuture_State_t MilkTea_TimerFuture_State_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerFuture_Destroy(MilkTea_TimerFuture_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerFuture_Cancel(MilkTea_TimerFuture_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerFuture_GetState(MilkTea_TimerFuture_t *self, MilkTea_TimerFuture_State_t *state);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerFuture_GetTime(MilkTea_TimerFuture_t *self, int64_t *time);

struct MilkTea_TimerTask_t;

#ifndef __cplusplus
typedef struct MilkTea_TimerTask_t MilkTea_TimerTask_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerTask_Destroy(MilkTea_TimerTask_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerTask_GetFuture(MilkTea_TimerTask_t *self, MilkTea_TimerFuture_t **future);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerTask_Run(MilkTea_TimerTask_t *self);

struct MilkTea_TimerWorker_t;

#ifndef __cplusplus
typedef struct MilkTea_TimerWorker_t MilkTea_TimerWorker_t;
#endif // ifndef __cplusplus

enum MilkTea_TimerWorker_State_t {
  MilkTea_TimerWorker_State_INIT,
  MilkTea_TimerWorker_State_RUNNING,
  MilkTea_TimerWorker_State_SHUTDOWN,
  MilkTea_TimerWorker_State_STOP,
  MilkTea_TimerWorker_State_TIDYING,
  MilkTea_TimerWorker_State_TERMINATED
};

#ifndef __cplusplus
typedef enum MilkTea_TimerWorker_State_t MilkTea_TimerWorker_State_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Create(MilkTea_TimerWorker_t **self, MilkTea_ClosureToken_t obj, bool (MilkTea_CALL *on_terminate)(void *obj, MilkTea_Exception_t exception, const char *what));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Destroy(MilkTea_TimerWorker_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Start(MilkTea_TimerWorker_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_GetState(MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Post(MilkTea_TimerWorker_t *self, int64_t delay, MilkTea_ClosureToken_t obj, void (MilkTea_CALL *action)(void *obj));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Shutdown(MilkTea_TimerWorker_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_ShutdownNow(MilkTea_TimerWorker_t *self, void *obj, void (MilkTea_CALL *callback)(void *obj, uint32_t size, MilkTea_TimerTask_t **tasks));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_AwaitTermination(MilkTea_TimerWorker_t *self, int64_t delay, bool *success);

struct MilkTea_TimerWorker_Weak_t;

#ifndef __cplusplus
typedef struct MilkTea_TimerWorker_Weak_t MilkTea_TimerWorker_Weak_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Weak_Create(MilkTea_TimerWorker_Weak_t **self, MilkTea_TimerWorker_t *target);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Weak_Destroy(MilkTea_TimerWorker_Weak_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Weak_Try(MilkTea_TimerWorker_Weak_t *self, MilkTea_TimerWorker_t **target);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKTEA_TIMER_H_
