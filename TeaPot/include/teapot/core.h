#ifndef LIB_TEAPOT_CORE_H_
#define LIB_TEAPOT_CORE_H_

#include <milktea.h>

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

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerFuture_Destroy(TeaPot_TimerFuture_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerFuture_Cancel(TeaPot_TimerFuture_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerFuture_GetState(TeaPot_TimerFuture_t *self, TeaPot_TimerFuture_State_t *state);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerFuture_GetTime(TeaPot_TimerFuture_t *self, int64_t *time);

struct TeaPot_TimerTask_t;

#ifndef __cplusplus
typedef struct TeaPot_TimerTask_t TeaPot_TimerTask_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerTask_Destroy(TeaPot_TimerTask_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerTask_GetFuture(TeaPot_TimerTask_t *self, TeaPot_TimerFuture_t **future);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerTask_Run(TeaPot_TimerTask_t *self);

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

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Create(TeaPot_TimerWorker_t **self, MilkTea_ClosureToken_t obj, bool (MilkTea_CALL *on_terminate)(void *obj, MilkTea_Exception_t exception, const char *what));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Destroy(TeaPot_TimerWorker_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Start(TeaPot_TimerWorker_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_GetState(TeaPot_TimerWorker_t *self, TeaPot_TimerWorker_State_t *state);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Post(TeaPot_TimerWorker_t *self, TeaPot_TimerFuture_t **future, int64_t delay, MilkTea_ClosureToken_t obj, void (MilkTea_CALL *action)(void *obj));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Shutdown(TeaPot_TimerWorker_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_ShutdownNow(TeaPot_TimerWorker_t *self, void *obj, void (MilkTea_CALL *callback)(void *obj, uint32_t size, TeaPot_TimerTask_t **tasks));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_AwaitTermination(TeaPot_TimerWorker_t *self, int64_t delay, bool *success);

struct TeaPot_TimerWorker_Weak_t;

#ifndef __cplusplus
typedef struct TeaPot_TimerWorker_Weak_t TeaPot_TimerWorker_Weak_t;
#endif // ifndef __cplusplus

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Weak_Create(TeaPot_TimerWorker_Weak_t **self, TeaPot_TimerWorker_t *target);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Weak_Destroy(TeaPot_TimerWorker_Weak_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
TeaPot_TimerWorker_Weak_Try(TeaPot_TimerWorker_Weak_t *self, TeaPot_TimerWorker_t **target);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

#include <chrono>

namespace TeaPot {

using clock_type = std::chrono::system_clock;
using duration_type = std::chrono::milliseconds;
using time_point_type = std::chrono::time_point<clock_type, duration_type>;

} // namespace TeaPot

#endif // ifdef __cplusplus

#endif // ifndef LIB_TEAPOT_CORE_H_
