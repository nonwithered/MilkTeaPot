#include <milktea.h>

namespace {

constexpr char TAG[] = "MilkTea#extern";

MilkTea_TimerWorker_State_t MilkTea_TimerWorker_State_Map(MilkTea::TimerWorker::State state) {
  switch (state) {
    case MilkTea::TimerWorker::State::INIT: return MilkTea_TimerWorker_State_INIT;
    case MilkTea::TimerWorker::State::RUNNING: return MilkTea_TimerWorker_State_RUNNING;
    case MilkTea::TimerWorker::State::SHUTDOWN: return MilkTea_TimerWorker_State_SHUTDOWN;
    case MilkTea::TimerWorker::State::STOP: return MilkTea_TimerWorker_State_STOP;
    case MilkTea::TimerWorker::State::TIDYING: return MilkTea_TimerWorker_State_TIDYING;
    case MilkTea::TimerWorker::State::TERMINATED: return MilkTea_TimerWorker_State_TERMINATED;
    default: MilkTea_assert("MilkTea_TimerWorker_GetState assert");
  }
}

} // namespace

extern "C" {

MilkTea_IMPL(MilkTea_TimerWorker_Create, (MilkTea_TimerWorker_t **self, void *obj, bool (MilkTea_CALL *on_terminate)(void *obj, MilkTea_Exception_t exception, const char *what)), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(obj);
  MilkTea_nonnull(on_terminate);
  auto *worker = new MilkTea::TimerWorker::worker_type(MilkTea::TimerWorker::Create([obj, on_terminate](std::exception *e) -> bool {
    if (e == nullptr) {
      return on_terminate(obj, MilkTea_Exception_t::MilkTea_Exception_Nil, nullptr);
    }
    MilkTea::Exception *exception = dynamic_cast<MilkTea::Exception *>(e);
    if (exception != nullptr) {
      return on_terminate(obj, exception->GetRawType(), exception->what());
    }
    return on_terminate(obj, MilkTea_Exception_t::MilkTea_Exception_Unknown, e->what());
  }));
  *self = reinterpret_cast<MilkTea_TimerWorker_t *>(worker);
})

MilkTea_IMPL(MilkTea_TimerWorker_Destroy, (MilkTea_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  auto *worker = reinterpret_cast<MilkTea::TimerWorker::worker_type *>(self);
  delete worker;
})

MilkTea_IMPL(MilkTea_TimerWorker_Start, (MilkTea_TimerWorker_t *self, bool *success), {
  MilkTea_nonnull(self);
  auto &worker = *reinterpret_cast<MilkTea::TimerWorker::worker_type *>(self);
  *success = worker->Start();
})

MilkTea_IMPL(MilkTea_TimerWorker_GetState, (MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  auto &worker = *reinterpret_cast<MilkTea::TimerWorker::worker_type *>(self);
  *state = MilkTea_TimerWorker_State_Map(worker->state());
})

MilkTea_IMPL(MilkTea_TimerWorker_Shutdown, (MilkTea_TimerWorker_t *self, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  auto &worker = *reinterpret_cast<MilkTea::TimerWorker::worker_type *>(self);
  *success = worker->Shutdown();
})

MilkTea_IMPL(MilkTea_TimerWorker_AwaitTermination, (MilkTea_TimerWorker_t *self, int64_t delay, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  auto &worker = *reinterpret_cast<MilkTea::TimerWorker::worker_type *>(self);
  if (delay > 0) {
    *success = worker->AwaitTermination(MilkTea::TimerWorker::duration_type(delay));
  } else {
    worker->AwaitTermination();
    *success = true;
  }
})

} // extern "C"
