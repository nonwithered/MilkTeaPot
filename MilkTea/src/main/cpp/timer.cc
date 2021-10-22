#include <milktea.h>

#include "timerworker.h"

namespace {

using namespace MilkTea;
using worker_type = MilkTea::TimerWorkerImpl::worker_type;
using task_type = MilkTea::TimerWorkerImpl::task_type;
using future_type = MilkTea::TimerWorkerImpl::future_type;
using clock_type = MilkTea::TimerWorkerImpl::clock_type;
using duration_type = MilkTea::TimerWorkerImpl::duration_type;
using time_point_type = MilkTea::TimerWorkerImpl::time_point_type;
using action_type = MilkTea::TimerWorkerImpl::action_type;

constexpr char TAG[] = "MilkTea_Timer_API";

void MilkTea_CALL MilkTea_TimerWorker_Create_run(void *action) {
  action_type *action_ = reinterpret_cast<action_type *>(action);
  MilkTea::Defer defer([action_]() -> void {
    delete action_;
  });
  (*action_)();
}

} // namespace

extern "C" {

MilkTea_IMPL(MilkTea_TimerWorker_Create, (MilkTea_TimerWorker_t **self, void *obj, void (MilkTea_CALL *callback)(void *obj, void *action, void (MilkTea_CALL *run)(void *action))), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(obj);
  MilkTea_nonnull(callback);
  worker_type worker_ = MilkTea::TimerWorkerImpl::Create([obj, callback](action_type action) -> void {
    callback(obj, new action_type(action), MilkTea_TimerWorker_Create_run);
  });
  *self = reinterpret_cast<MilkTea_TimerWorker_t *>(new worker_type(std::move(worker_)));
})

MilkTea_IMPL(MilkTea_TimerWorker_Refer, (MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_t **worker), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(worker);
  worker_type worker_ = *reinterpret_cast<worker_type *>(self);
  *worker = reinterpret_cast<MilkTea_TimerWorker_t *>(new worker_type(std::move(worker_)));
})

MilkTea_IMPL(MilkTea_TimerWorker_Destroy, (MilkTea_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  delete reinterpret_cast<worker_type *>(self);
})

MilkTea_IMPL(MilkTea_TimerWorker_GetState, (MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  switch ((*reinterpret_cast<worker_type *>(self))->state()) {
    case MilkTea::TimerWorkerImpl::State::INIT: *state = MilkTea_TimerWorker_State_INIT; break;
    case MilkTea::TimerWorkerImpl::State::RUNNING: *state = MilkTea_TimerWorker_State_RUNNING; break;
    case MilkTea::TimerWorkerImpl::State::SHUTDOWN: *state = MilkTea_TimerWorker_State_SHUTDOWN; break;
    case MilkTea::TimerWorkerImpl::State::STOP: *state = MilkTea_TimerWorker_State_STOP; break;
    case MilkTea::TimerWorkerImpl::State::TIDYING: *state = MilkTea_TimerWorker_State_TIDYING; break;
    case MilkTea::TimerWorkerImpl::State::TERMINATED: *state = MilkTea_TimerWorker_State_TERMINATED; break;
    default: MilkTea_throw(Assertion, "TimerWorker GetState assert");
  }
})

} // extern "C"
