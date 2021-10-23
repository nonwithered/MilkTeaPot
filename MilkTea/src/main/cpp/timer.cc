#include <milktea.h>

namespace {

constexpr char TAG[] = "MilkTea_Timer_API";

} // namespace

extern "C" {

MilkTea_IMPL(MilkTea_TimerWorker_Create, (MilkTea_TimerWorker_t **self, void *obj, bool (MilkTea_CALL *on_terminate)(void *obj, MilkTea_Exception_t exception, const char *what)), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(obj);
  MilkTea_nonnull(on_terminate);
  *self = reinterpret_cast<MilkTea_TimerWorker_t *>(new MilkTea::TimerWorker([obj, on_terminate](std::exception *e) -> bool {
    if (e == nullptr) {
      return on_terminate(obj, MilkTea_Exception_t::MilkTea_Exception_Nil, nullptr);
    }
    MilkTea::Exception *exception = dynamic_cast<MilkTea::Exception *>(e);
    if (exception != nullptr) {
      return on_terminate(obj, exception->GetRawType(), exception->what());
    }
    return on_terminate(obj, MilkTea_Exception_t::MilkTea_Exception_Unknown, e->what());
  }));
})

MilkTea_IMPL(MilkTea_TimerWorker_Destroy, (MilkTea_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  delete reinterpret_cast<MilkTea::TimerWorker *>(self);
})

MilkTea_IMPL(MilkTea_TimerWorker_Start, (MilkTea_TimerWorker_t *self, bool *success), {
  MilkTea_nonnull(self);
  *success = reinterpret_cast<MilkTea::TimerWorker *>(self)->Start();
})

MilkTea_IMPL(MilkTea_TimerWorker_GetState, (MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  switch (reinterpret_cast<MilkTea::TimerWorker *>(self)->state()) {
    case MilkTea::TimerWorker::State::INIT: *state = MilkTea_TimerWorker_State_INIT; break;
    case MilkTea::TimerWorker::State::RUNNING: *state = MilkTea_TimerWorker_State_RUNNING; break;
    case MilkTea::TimerWorker::State::SHUTDOWN: *state = MilkTea_TimerWorker_State_SHUTDOWN; break;
    case MilkTea::TimerWorker::State::STOP: *state = MilkTea_TimerWorker_State_STOP; break;
    case MilkTea::TimerWorker::State::TIDYING: *state = MilkTea_TimerWorker_State_TIDYING; break;
    case MilkTea::TimerWorker::State::TERMINATED: *state = MilkTea_TimerWorker_State_TERMINATED; break;
    default: MilkTea_throw(Assertion, "MilkTea_TimerWorker_GetState assert");
  }
})

MilkTea_IMPL(MilkTea_TimerWorker_Shutdown, (MilkTea_TimerWorker_t *self, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  *success = reinterpret_cast<MilkTea::TimerWorker *>(self)->Shutdown();
})

MilkTea_IMPL(MilkTea_TimerWorker_AwaitTermination, (MilkTea_TimerWorker_t *self, int64_t delay, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  if (delay > 0) {
    *success = reinterpret_cast<MilkTea::TimerWorker *>(self)->AwaitTermination(MilkTea::TimerWorker::duration_type(delay));
  } else {
    reinterpret_cast<MilkTea::TimerWorker *>(self)->AwaitTermination();
    *success = true;
  }
})

} // extern "C"
