#include "timerworker.h"

using duration_type = TeaPot::TimerUnit::duration_type;

namespace {

constexpr char TAG[] = "TeaPot";

template<typename T>
struct timer_cast_map;

template<>
struct timer_cast_map<TeaPot_TimerFuture_t> {
  using target = TeaPot::future_type;
};

template<>
struct timer_cast_map<TeaPot_TimerTask_t> {
  using target = TeaPot::task_type;
};

template<>
struct timer_cast_map<TeaPot_TimerWorker_t> {
  using target = TeaPot::worker_type;
};

template<>
struct timer_cast_map<TeaPot_TimerWorker_Weak_t> {
  using target = TeaPot::worker_weak;
};

template<typename T>
typename timer_cast_map<T>::target &timer_cast(T *self) {
  return *reinterpret_cast<typename timer_cast_map<T>::target *>(self);
}

template<>
struct timer_cast_map<TeaPot::future_type> {
  using target = TeaPot_TimerFuture_t;
};

template<>
struct timer_cast_map<TeaPot::task_type::element_type> {
  using target = TeaPot_TimerTask_t;
};

template<>
struct timer_cast_map<TeaPot::worker_type> {
  using target = TeaPot_TimerWorker_t;
};

template<>
struct timer_cast_map<TeaPot::worker_weak> {
  using target = TeaPot_TimerWorker_Weak_t;
};

template<typename T>
typename timer_cast_map<T>::target *timer_cast(T &self) {
  return reinterpret_cast<typename timer_cast_map<T>::target *>(&self);
}

} // namespace

MilkTea_extern(TeaPot_TimerFuture_Destroy, (TeaPot_TimerFuture_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
});

MilkTea_extern(TeaPot_TimerFuture_Cancel, (TeaPot_TimerFuture_t *self, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  *success = timer_cast(self)->Cancel();
});

MilkTea_extern(TeaPot_TimerFuture_GetState, (TeaPot_TimerFuture_t *self, TeaPot_TimerFuture_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  *state = TeaPot::TimerFuture::ToRawType(timer_cast(self)->state());
});

MilkTea_extern(TeaPot_TimerFuture_GetTime, (TeaPot_TimerFuture_t *self, int64_t *time), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(time);
  *time = timer_cast(self)->time().time_since_epoch().count();
});

MilkTea_extern(TeaPot_TimerTask_Destroy, (TeaPot_TimerTask_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
});

MilkTea_extern(TeaPot_TimerTask_GetFuture, (TeaPot_TimerTask_t *self, TeaPot_TimerFuture_t **future), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(future);
  *future = timer_cast(*new TeaPot::future_type(timer_cast(self)->future()));
});

MilkTea_extern(TeaPot_TimerTask_Run, (TeaPot_TimerTask_t *self), {
  MilkTea_nonnull(self);
  timer_cast(self)->Run();
});

MilkTea_extern(TeaPot_TimerWorker_Create, (TeaPot_TimerWorker_t **self, TeaPot_TimerWorker_Termination termination), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(termination.invoke_);
  auto worker = TeaPot::TimerWorkerImpl::Make([termination_ =
    MilkTea::FunctionAdapter::FromRawType<TeaPot_TimerWorker_Termination>(termination)]
  (auto type, auto what) -> bool {
    return termination_(MilkTea::Exception::ToRawType(type), what.data());
  });
  *self = timer_cast(*new TeaPot::worker_type(std::move(worker)));
})

MilkTea_extern(TeaPot_TimerWorker_Destroy, (TeaPot_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
})

MilkTea_extern(TeaPot_TimerWorker_Close, (TeaPot_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  auto worker = std::move(timer_cast(self));
  delete &timer_cast(self);
  TeaPot::worker_raw::Close(std::move(worker));
})

MilkTea_extern(TeaPot_TimerWorker_Start, (TeaPot_TimerWorker_t *self), {
  MilkTea_nonnull(self);
  timer_cast(self)->Start();
})

MilkTea_extern(TeaPot_TimerWorker_GetState, (TeaPot_TimerWorker_t *self, TeaPot_TimerWorker_State_t *state), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(state);
  *state = TeaPot::TimerWorker::ToRawType(timer_cast(self)->state());
})

MilkTea_extern(TeaPot_TimerWorker_Post, (TeaPot_TimerWorker_t *self, TeaPot_TimerFuture_t **future, int64_t delay, TeaPot_Action_t action), {
  MilkTea_nonnull(self);
  if (delay < 0) {
    MilkTea_throwf(InvalidParam, "post -- delay: %" PRId64, delay);
  }
  auto future_ = timer_cast(self)->Post(duration_type(delay), TeaPot::Action::FromRawType(action));
  if (future != nullptr) {
    *future = timer_cast(*new TeaPot::future_type(std::move(future_)));
  }
});

MilkTea_extern(TeaPot_TimerWorker_Shutdown, (TeaPot_TimerWorker_t *self, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  *success = timer_cast(self)->Shutdown();
})

MilkTea_extern(TeaPot_TimerWorker_ShutdownNow, (TeaPot_TimerWorker_t *self, TeaPot_TimerTask_Consumer_t consumer), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(consumer.invoke_);
  bool success;
  std::vector<TeaPot::task_type> vec;
  std::tie(success, vec) = timer_cast(self)->ShutdownNow();
  if (success) {
    std::for_each(vec.begin(), vec.end(), [consumer](TeaPot::task_type &it) {
      MilkTea_Function_Invoke(consumer, timer_cast(*it.release()));
    });
  }
});

MilkTea_extern(TeaPot_TimerWorker_AwaitTermination, (TeaPot_TimerWorker_t *self, int64_t delay, bool *success), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(success);
  if (delay < 0) {
    MilkTea_throwf(InvalidParam, "post -- delay: %" PRId64, delay);
  }
  if (delay > 0) {
    *success = timer_cast(self)->AwaitTermination(duration_type(delay));
  } else {
    timer_cast(self)->AwaitTermination();
    *success = true;
  }
})

MilkTea_extern(TeaPot_TimerWorker_Weak_Create, (TeaPot_TimerWorker_Weak_t **self, TeaPot_TimerWorker_t *target), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(target);
  *self = timer_cast(*new TeaPot::worker_weak(timer_cast(target)));
})

MilkTea_extern(TeaPot_TimerWorker_Weak_Destroy, (TeaPot_TimerWorker_Weak_t *self), {
  MilkTea_nonnull(self);
  delete &timer_cast(self);
})

MilkTea_extern(TeaPot_TimerWorker_Weak_Try, (TeaPot_TimerWorker_Weak_t *self, TeaPot_TimerWorker_t **target), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(target);
  auto strong = timer_cast(self).lock();
  if (strong.get() != nullptr) {
    *target = timer_cast(*new TeaPot::worker_type(std::move(strong)));
  } else {
    *target = nullptr;
  }
})

namespace TeaPot {

manager_type &TimerManagerImpl::Instance() {
  static manager_type instance_{};
  return instance_;
}

} // namespace TeaPot
