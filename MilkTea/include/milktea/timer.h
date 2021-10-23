#ifndef LIB_MILKTEA_TIMER_H_
#define LIB_MILKTEA_TIMER_H_

#include <milktea/exception.h>
#include <milktea/timerworker.h>

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

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Create(MilkTea_TimerWorker_t **self, void *obj, bool (MilkTea_CALL *on_terminate)(void *obj, MilkTea_Exception_t exception, const char *what));

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Destroy(MilkTea_TimerWorker_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Start(MilkTea_TimerWorker_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_GetState(MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_Shutdown(MilkTea_TimerWorker_t *self, bool *success);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
MilkTea_TimerWorker_AwaitTermination(MilkTea_TimerWorker_t *self, int64_t delay, bool *success);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKTEA_TIMER_H_
