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
typename timer_cast_map<T>::target &timer_cast(T *obj) {
  return *reinterpret_cast<typename timer_cast_map<T>::target *>(obj);
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
typename timer_cast_map<T>::target *timer_cast(T &obj) {
  return reinterpret_cast<typename timer_cast_map<T>::target *>(&obj);
}

} // namespace

MilkTea_extern(TeaPot_TimerFuture_Destroy, (TeaPot_TimerFuture_t *obj), {
  MilkTea_nonnull(obj);
  delete &timer_cast(obj);
});

MilkTea_extern(TeaPot_TimerFuture_Cancel, (TeaPot_TimerFuture_t *obj, bool *success), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(success);
  *success = timer_cast(obj)->Cancel();
});

MilkTea_extern(TeaPot_TimerFuture_GetState, (TeaPot_TimerFuture_t *obj, TeaPot_TimerFuture_State_t *state), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(state);
  *state = TeaPot::TimerFuture::ToRawType(timer_cast(obj)->state());
});

MilkTea_extern(TeaPot_TimerFuture_GetTime, (TeaPot_TimerFuture_t *obj, int64_t *time), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(time);
  *time = timer_cast(obj)->time().time_since_epoch().count();
});

MilkTea_extern(TeaPot_TimerTask_Destroy, (TeaPot_TimerTask_t *obj), {
  MilkTea_nonnull(obj);
  delete &timer_cast(obj);
});

MilkTea_extern(TeaPot_TimerTask_GetFuture, (TeaPot_TimerTask_t *obj, TeaPot_TimerFuture_t **future), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(future);
  *future = timer_cast(*new TeaPot::future_type(timer_cast(obj)->future()));
});

MilkTea_extern(TeaPot_TimerTask_Run, (TeaPot_TimerTask_t *obj), {
  MilkTea_nonnull(obj);
  timer_cast(obj)->Run();
});

MilkTea_extern(TeaPot_TimerWorker_Create, (TeaPot_TimerWorker_t **obj, TeaPot_TimerWorker_Termination termination), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(termination.invoke_);
  auto worker = TeaPot::TimerWorkerImpl::Make([termination_ =
    MilkTea::FunctionAdapter::FromRawType<TeaPot_TimerWorker_Termination>(termination)]
  (auto type, auto what) -> bool {
    return termination_(MilkTea::Exception::ToRawType(type), what.data());
  });
  *obj = timer_cast(*new TeaPot::worker_type(std::move(worker)));
})

MilkTea_extern(TeaPot_TimerWorker_Destroy, (TeaPot_TimerWorker_t *obj), {
  MilkTea_nonnull(obj);
  delete &timer_cast(obj);
})

MilkTea_extern(TeaPot_TimerWorker_Close, (TeaPot_TimerWorker_t *obj), {
  MilkTea_nonnull(obj);
  auto worker = std::move(timer_cast(obj));
  delete &timer_cast(obj);
  TeaPot::worker_raw::Close(std::move(worker));
})

MilkTea_extern(TeaPot_TimerWorker_Start, (TeaPot_TimerWorker_t *obj), {
  MilkTea_nonnull(obj);
  timer_cast(obj)->Start();
})

MilkTea_extern(TeaPot_TimerWorker_GetState, (TeaPot_TimerWorker_t *obj, TeaPot_TimerWorker_State_t *state), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(state);
  *state = TeaPot::TimerWorker::ToRawType(timer_cast(obj)->state());
})

MilkTea_extern(TeaPot_TimerWorker_Post, (TeaPot_TimerWorker_t *obj, TeaPot_TimerFuture_t **future, int64_t delay, TeaPot_Action_t action), {
  MilkTea_nonnull(obj);
  if (delay < 0) {
    MilkTea_throwf(InvalidParam, "post -- delay: %" PRId64, delay);
  }
  auto future_ = timer_cast(obj)->Post(duration_type(delay), TeaPot::Action::FromRawType(action));
  if (future != nullptr) {
    *future = timer_cast(*new TeaPot::future_type(std::move(future_)));
  }
});

MilkTea_extern(TeaPot_TimerWorker_Shutdown, (TeaPot_TimerWorker_t *obj, bool *success), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(success);
  *success = timer_cast(obj)->Shutdown();
})

MilkTea_extern(TeaPot_TimerWorker_ShutdownNow, (TeaPot_TimerWorker_t *obj, TeaPot_TimerTask_Consumer_t consumer), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(consumer.invoke_);
  bool success;
  std::vector<TeaPot::task_type> vec;
  std::tie(success, vec) = timer_cast(obj)->ShutdownNow();
  if (success) {
    std::for_each(vec.begin(), vec.end(), [consumer](TeaPot::task_type &it) {
      MilkTea_Function_Invoke(consumer, timer_cast(*it.release()));
    });
  }
});

MilkTea_extern(TeaPot_TimerWorker_AwaitTermination, (TeaPot_TimerWorker_t *obj, int64_t delay, bool *success), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(success);
  if (delay < 0) {
    MilkTea_throwf(InvalidParam, "post -- delay: %" PRId64, delay);
  }
  if (delay > 0) {
    *success = timer_cast(obj)->AwaitTermination(duration_type(delay));
  } else {
    timer_cast(obj)->AwaitTermination();
    *success = true;
  }
})

MilkTea_extern(TeaPot_TimerWorker_Weak_Create, (TeaPot_TimerWorker_Weak_t **obj, TeaPot_TimerWorker_t *target), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(target);
  *obj = timer_cast(*new TeaPot::worker_weak(timer_cast(target)));
})

MilkTea_extern(TeaPot_TimerWorker_Weak_Destroy, (TeaPot_TimerWorker_Weak_t *obj), {
  MilkTea_nonnull(obj);
  delete &timer_cast(obj);
})

MilkTea_extern(TeaPot_TimerWorker_Weak_Try, (TeaPot_TimerWorker_Weak_t *obj, TeaPot_TimerWorker_t **target), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(target);
  auto strong = timer_cast(obj).lock();
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
