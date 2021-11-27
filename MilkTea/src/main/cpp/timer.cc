#include <milktea.h>

#include "timerworker.h"

namespace {

constexpr char TAG[] = "MilkTea#extern";

MilkTea_TimerFuture_State_t MilkTea_TimerFuture_State_Map(MilkTea::TimerFutureImpl::State state) {
  switch (state) {
    case MilkTea::TimerFutureImpl::State::SCHEDULED: return MilkTea_TimerFuture_State_SCHEDULED;
    case MilkTea::TimerFutureImpl::State::EXECUTED: return MilkTea_TimerFuture_State_EXECUTED;
    case MilkTea::TimerFutureImpl::State::CANCELLED: return MilkTea_TimerFuture_State_CANCELLED;
    case MilkTea::TimerFutureImpl::State::NORMAL: return MilkTea_TimerFuture_State_NORMAL;
    case MilkTea::TimerFutureImpl::State::EXCEPTIONAL: return MilkTea_TimerFuture_State_EXCEPTIONAL;
    default: MilkTea_assert("MilkTea_TimerFuture_GetState assert");
  }
}

MilkTea_TimerWorker_State_t MilkTea_TimerWorker_State_Map(MilkTea::TimerWorkerImpl::State state) {
  switch (state) {
    case MilkTea::TimerWorkerImpl::State::INIT: return MilkTea_TimerWorker_State_INIT;
    case MilkTea::TimerWorkerImpl::State::RUNNING: return MilkTea_TimerWorker_State_RUNNING;
    case MilkTea::TimerWorkerImpl::State::SHUTDOWN: return MilkTea_TimerWorker_State_SHUTDOWN;
    case MilkTea::TimerWorkerImpl::State::STOP: return MilkTea_TimerWorker_State_STOP;
    case MilkTea::TimerWorkerImpl::State::TIDYING: return MilkTea_TimerWorker_State_TIDYING;
    case MilkTea::TimerWorkerImpl::State::TERMINATED: return MilkTea_TimerWorker_State_TERMINATED;
    default: MilkTea_assert("MilkTea_TimerWorker_GetState assert");
  }
}

MilkTea_TimerWorker_typedef

template<typename T>
struct timer_cast_map;

template<>
struct timer_cast_map<MilkTea_TimerFuture_t> {
  using Type = future_type;
};

template<>
struct timer_cast_map<MilkTea_TimerTask_t> {
  using Type = task_type;
};

template<>
struct timer_cast_map<MilkTea_TimerWorker_t> {
  using Type = worker_type;
};

template<>
struct timer_cast_map<MilkTea_TimerWorker_Weak_t> {
  using Type = worker_weak;
};

template<typename T>
typename timer_cast_map<T>::Type &timer_cast(T *self) {
  return *reinterpret_cast<typename timer_cast_map<T>::Type *>(self);
}

template<>
struct timer_cast_map<MilkTea::TimerFutureImpl::future_type> {
  using Type = MilkTea_TimerFuture_t;
};

template<>
struct timer_cast_map<MilkTea::TimerTaskImpl::task_type::element_type> {
  using Type = MilkTea_TimerTask_t;
};

template<>
struct timer_cast_map<worker_type> {
  using Type = MilkTea_TimerWorker_t;
};

template<>
struct timer_cast_map<worker_weak> {
  using Type = MilkTea_TimerWorker_Weak_t;
};

template<typename T>
typename timer_cast_map<T>::Type *timer_cast(T &self) {
  return reinterpret_cast<typename timer_cast_map<T>::Type *>(&self);
}

} // namespace

extern "C" {

MilkTea_IMPL(MilkTea_TimerFuture_Destroy, (MilkTea_TimerFuture_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
});

MilkTea_IMPL(MilkTea_TimerFuture_Cancel, (MilkTea_TimerFuture_t *self, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  *success = timer_cast(self)->Cancel();
});

MilkTea_IMPL(MilkTea_TimerFuture_GetState, (MilkTea_TimerFuture_t *self, MilkTea_TimerFuture_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  *state = MilkTea_TimerFuture_State_Map(timer_cast(self)->state());
});

MilkTea_IMPL(MilkTea_TimerFuture_GetTime, (MilkTea_TimerFuture_t *self, int64_t *time), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(time);
  *time = timer_cast(self)->time().time_since_epoch().count();
});

MilkTea_IMPL(MilkTea_TimerTask_Destroy, (MilkTea_TimerTask_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
});

MilkTea_IMPL(MilkTea_TimerTask_GetFuture, (MilkTea_TimerTask_t *self, MilkTea_TimerFuture_t **future), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(future);
  *future = timer_cast(*new future_type(timer_cast(self)->future()));
});

MilkTea_IMPL(MilkTea_TimerTask_Run, (MilkTea_TimerTask_t *self), {
  MilkTea_nonnull(self);
  timer_cast(self)->Run();
});

MilkTea_IMPL(MilkTea_TimerWorker_Create, (MilkTea_TimerWorker_t **self, MilkTea_ClosureToken_t obj, bool (MilkTea_CALL *on_terminate)(void *obj, MilkTea_Exception_t exception, const char *what)), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(on_terminate);
  auto do_terminate = MilkTea::ClosureToken<bool(MilkTea_Exception_t, const char *)>::FromRawType(obj, on_terminate);
  auto worker = MilkTea::TimerWorkerImpl::Make([do_terminate](std::exception *e) -> bool {
    if (e == nullptr) {
      return do_terminate(MilkTea_Exception_t::MilkTea_Exception_Nil, nullptr);
    }
    MilkTea::Exception *exception = dynamic_cast<MilkTea::Exception *>(e);
    if (exception != nullptr) {
      return do_terminate(exception->GetRawType(), exception->what());
    }
    return do_terminate(MilkTea_Exception_t::MilkTea_Exception_Unknown, e->what());
  });
  *self = timer_cast(*new worker_type(std::move(worker)));
})

MilkTea_IMPL(MilkTea_TimerWorker_Destroy, (MilkTea_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  auto worker = std::move(timer_cast(self));
  delete &timer_cast(self);
  worker_raw::Close(std::move(worker));
})

MilkTea_IMPL(MilkTea_TimerWorker_Start, (MilkTea_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  timer_cast(self)->Start();
})

MilkTea_IMPL(MilkTea_TimerWorker_GetState, (MilkTea_TimerWorker_t *self, MilkTea_TimerWorker_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  *state = MilkTea_TimerWorker_State_Map(timer_cast(self)->state());
})

MilkTea_IMPL(MilkTea_TimerWorker_Post, (MilkTea_TimerWorker_t *self, int64_t delay, MilkTea_ClosureToken_t obj, void (MilkTea_CALL *action)(void *obj)), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(action);
  if (delay < 0) {
    MilkTea_throwf(InvalidParam, "post -- delay: %" PRIi64, delay);
  }
  auto do_action = MilkTea::ClosureToken<void()>::FromRawType(obj, action);
  timer_cast(self)->Post(duration_type(delay), do_action);
});

MilkTea_IMPL(MilkTea_TimerWorker_Shutdown, (MilkTea_TimerWorker_t *self, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  *success = timer_cast(self)->Shutdown();
})

MilkTea_IMPL(MilkTea_TimerWorker_ShutdownNow, (MilkTea_TimerWorker_t *self, void *obj, void (MilkTea_CALL *callback)(void *obj, uint32_t size, MilkTea_TimerTask_t **tasks)), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(callback);
  bool success;
  std::vector<task_type> vec;
  std::tie(success, vec) = timer_cast(self)->ShutdownNow();
  if (success) {
    auto sz = vec.size();
    std::vector<MilkTea_TimerTask_t *> arr(sz);
    for (decltype(sz) i = 0; i != sz; ++i) {
      arr[i] = timer_cast(*vec[i].release());
    }
    callback(obj, static_cast<uint32_t>(sz), arr.data());
  }
});

MilkTea_IMPL(MilkTea_TimerWorker_AwaitTermination, (MilkTea_TimerWorker_t *self, int64_t delay, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  if (delay < 0) {
    MilkTea_throwf(InvalidParam, "post -- delay: %" PRIi64, delay);
  }
  if (delay > 0) {
    *success = timer_cast(self)->AwaitTermination(duration_type(delay));
  } else {
    timer_cast(self)->AwaitTermination();
    *success = true;
  }
})

MilkTea_IMPL(MilkTea_TimerWorker_Weak_Create, (MilkTea_TimerWorker_Weak_t **self, MilkTea_TimerWorker_t *target), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(target);
  *self = timer_cast(*new worker_weak(timer_cast(target)));
})

MilkTea_IMPL(MilkTea_TimerWorker_Weak_Destroy, (MilkTea_TimerWorker_Weak_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
})

MilkTea_IMPL(MilkTea_TimerWorker_Weak_Try, (MilkTea_TimerWorker_Weak_t *self, MilkTea_TimerWorker_t **target), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(target);
  auto strong = timer_cast(self).lock();
  if (strong.get() != nullptr) {
    *target = timer_cast(*new worker_type(std::move(strong)));
  } else {
    *target = nullptr;
  }
})

} // extern "C"

namespace MilkTea {

manager_type &TimerManagerImpl::Instance() {
  static manager_type instance_{};
  return instance_;
}

} // namespace MilkTea
