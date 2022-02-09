#ifndef LIB_TEAPOT_WRAPPER_TIMERWORKER_H_
#define LIB_TEAPOT_WRAPPER_TIMERWORKER_H_

#include <functional>
#include <exception>
#include <vector>
#include <tuple>

#include <teapot/common.h>
#include <teapot/wrapper/timertask.h>

namespace TeaPot {

class TimerWorkerWrapper final {
  static constexpr char TAG[] = "TeaPot::TimerWorkerWrapper";
  using State = TeaPot::TimerWorker::State;
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  using raw_type = TeaPot_TimerWorker_t;
  explicit TimerWorkerWrapper(std::function<bool(MilkTea::Exception::Type, std::string_view)> termination) : TimerWorkerWrapper() {
    std::function<bool(MilkTea_Exception_t, const char *)> termination_ = [termination](MilkTea_Exception_t type, const char *what) -> bool {
      return termination(MilkTea::Exception::FromRawType(type), what);
    };
    raw_type *obj = nullptr;
    MilkTea_invoke_panic(TeaPot_TimerWorker_Create, &obj, MilkTea::FunctionAdapter::ToRawType<TeaPot_TimerWorker_Termination>(termination_));
    std::swap(obj_, obj);
  }
  TimerWorkerWrapper(raw_type *obj = nullptr) : obj_(obj) {}
  TimerWorkerWrapper(TimerWorkerWrapper &&another) : TimerWorkerWrapper() {
    std::swap(obj_, another.obj_);
  }
  ~TimerWorkerWrapper() {
    auto obj = release();
    if (obj == nullptr) {
      return;
    }
    MilkTea_invoke_panic(TeaPot_TimerWorker_Destroy, obj);
  }
  void Close() {
    auto obj = release();
    if (obj == nullptr) {
      return;
    }
    MilkTea_invoke_panic(TeaPot_TimerWorker_Close, obj);
  }
  void Start() {
    MilkTea_invoke_panic(TeaPot_TimerWorker_Start, get());
  }
  State GetState() {
    TeaPot_TimerWorker_State_t state = TeaPot_TimerWorker_State_CLOSED;
    MilkTea_invoke_panic(TeaPot_TimerWorker_GetState, get(), &state);
    return TimerWorker::FromRawType(state);
  }
  TimerFutureWrapper Post(Action::action_type action, duration_type delay = duration_type::zero()) {
    TeaPot_TimerFuture_t *future = nullptr;
    MilkTea_invoke_panic(TeaPot_TimerWorker_Post, get(), &future, delay.count(), Action::ToRawType(action));
    return future;
  }
  bool Shutdown() {
    bool success = false;
    MilkTea_invoke_panic(TeaPot_TimerWorker_Shutdown, get(), &success);
    return success;
  }
  std::tuple<bool, std::vector<TimerTaskWrapper>> ShutdownNow() {
    bool success = false;
    std::vector<TimerTaskWrapper> vec;
    std::function<void(TeaPot_TimerTask_t *)> consumer = [&success, &vec](TeaPot_TimerTask_t *task) -> void {
      success = true;
      vec.emplace_back(task);
    };
    MilkTea_invoke_panic(TeaPot_TimerWorker_ShutdownNow, get(), MilkTea::FunctionAdapter::ToRawType<TeaPot_TimerTask_Consumer_t>(consumer));
    return std::make_tuple(success, std::move(vec));
  }
  bool AwaitTermination(duration_type delay = duration_type()) {
    bool success = false;
    MilkTea_invoke_panic(TeaPot_TimerWorker_AwaitTermination, get(), delay.count(), &success);
    return success;
  }
  raw_type *get() const {
    return obj_;
  }
  operator bool() const {
    return get() != nullptr;
  }
  raw_type *release() {
    raw_type *obj = obj_;
    obj_ = nullptr;
    return obj;
  }
 private:
  raw_type *obj_;
  MilkTea_NonCopy(TimerWorkerWrapper)
  MilkTea_NonMoveAssign(TimerWorkerWrapper)
};

class TimerWorkerWeakWrapper final {
 public:
  using raw_type = TeaPot_TimerWorker_Weak_t;
  TimerWorkerWeakWrapper(raw_type *obj = nullptr) : obj_(obj) {}
  TimerWorkerWeakWrapper(const TimerWorkerWrapper &another) : TimerWorkerWeakWrapper() {
    MilkTea_invoke_panic(TeaPot_TimerWorker_Weak_Create, &obj_, another.get());
  }
  TimerWorkerWeakWrapper(TimerWorkerWeakWrapper &&another) : TimerWorkerWeakWrapper() {
    std::swap(obj_, another.obj_);
  }
  ~TimerWorkerWeakWrapper() {
    if (obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(TeaPot_TimerWorker_Weak_Destroy, get());
    obj_ = nullptr;
  }
  TimerWorkerWrapper lock() {
    TimerWorkerWrapper::raw_type *lock_ = nullptr;
    MilkTea_invoke_panic(TeaPot_TimerWorker_Weak_Try, get(), &lock_);
    return lock_;
  }
  raw_type *get() const {
    return obj_;
  }
  operator bool() const {
    return get() != nullptr;
  }
  raw_type *release() {
    raw_type *obj = obj_;
    obj_ = nullptr;
    return obj;
  }
 private:
  raw_type *obj_;
  MilkTea_NonCopy(TimerWorkerWeakWrapper)
  MilkTea_NonMoveAssign(TimerWorkerWeakWrapper)
};

} // namespace TeaPot

#endif // ifndef LIB_TEAPOT_WRAPPER_TIMERWORKER_H_
