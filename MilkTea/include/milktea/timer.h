#ifndef LIB_MILKTEA_TIMER_H_
#define LIB_MILKTEA_TIMER_H_

#include <milktea/exception.h>

#ifdef __cplusplus
extern "C" {
#endif

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

struct MilkTea_TimerTask_t;

#ifndef __cplusplus
typedef struct MilkTea_TimerTask_t MilkTea_TimerTask_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Create(MilkTea_TimerWorker_t **self, void *obj, void (MilkTea_CALL *callback)(void *obj, void *worker, void (MilkTea_CALL *run)(void *worker)));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Refer(MilkTea_TimerWorker_t **self, MilkTea_TimerWorker_t *worker);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Destroy(MilkTea_TimerWorker_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_GetState(MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_AwaitTermination(MilkTea_TimerWorker_t *self, int64_t delay, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Shutdown(MilkTea_TimerWorker_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_ShutdownNow(MilkTea_TimerWorker_t *self, void *obj, void (MilkTea_CALL *callback)(void *obj, uint32_t len, MilkTea_TimerTask_t *tasks[]));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Post(MilkTea_TimerWorker_t *self, int64_t delay, void *task, uint32_t size, void (MilkTea_CALL *deletor)(void *task), void (MilkTea_CALL *run)(void *task));

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
namespace MilkTea {

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMER_H_
