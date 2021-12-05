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
  explicit TimerWorkerWrapper(std::function<bool(std::exception *)> on_terminate) : TimerWorkerWrapper() {
    std::function<bool(MilkTea_Exception_t, const char *)> do_terminate = [on_terminate](MilkTea_Exception_t type, const char *what) -> bool {
      if (type == MilkTea_Exception_Nil) {
        return on_terminate(nullptr);
      }
      try {
        MilkTea_Exception_Throw(type, what);
      }  catch (std::exception &e) {
        return on_terminate(&e);
      }
      return false;
    };
    TeaPot_TimerWorker_t *self = nullptr;
    MilkTea_panic(TeaPot_TimerWorker_Create(&self, MilkTea::ClosureToken<decltype(do_terminate)>::ToRawType(do_terminate), MilkTea::ClosureToken<decltype(do_terminate)>::Invoke));
    std::swap(self_, self);
  }
  TimerWorkerWrapper(TeaPot_TimerWorker_t *self = nullptr) : self_(self) {}
  TimerWorkerWrapper(TimerWorkerWrapper &&another) : TimerWorkerWrapper() {
    std::swap(self_, another.self_);
  }
  ~TimerWorkerWrapper() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_panic(TeaPot_TimerWorker_Destroy(self_));
    self_ = nullptr;
  }
  void Start() {
    MilkTea_panic(TeaPot_TimerWorker_Start(self_));
  }
  State GetState() {
    TeaPot_TimerWorker_State_t state = TeaPot_TimerWorker_State_CLOSED;
    MilkTea_panic(TeaPot_TimerWorker_GetState(self_, &state));
    return TimerWorker::FromRawType(state);
  }
  TimerFutureWrapper Post(duration_type delay, std::function<void()> action) {
    TeaPot_TimerFuture_t *future = nullptr;
    MilkTea_panic(TeaPot_TimerWorker_Post(self_, &future, delay.count(), MilkTea::ClosureToken<decltype(action)>::ToRawType(action), MilkTea::ClosureToken<decltype(action)>::Invoke));
    return future;
  }
  bool Shutdown() {
    bool success = false;
    MilkTea_panic(TeaPot_TimerWorker_Shutdown(self_, &success));
    return success;
  }
  std::tuple<bool, std::vector<TimerTaskWrapper>> ShutdownNow() {
    bool success = false;
    std::vector<TimerTaskWrapper> vec;
    std::function<void(uint32_t, TeaPot_TimerTask_t **)> callback = [&success, &vec](uint32_t size, TeaPot_TimerTask_t ** tasks) -> void {
      success = true;
      for (uint32_t i = 0; i != size; ++i) {
        vec.emplace_back(tasks[i]);
      }
    };
    MilkTea_panic(TeaPot_TimerWorker_ShutdownNow(self_, &callback, MilkTea::ClosureToken<decltype(callback)>::Invoke));
    return std::make_tuple(success, std::move(vec));
  }
  bool AwaitTermination(duration_type delay = duration_type()) {
    bool success = false;
    MilkTea_panic(TeaPot_TimerWorker_AwaitTermination(self_, delay.count(), &success));
    return success;
  }
 private:
  TeaPot_TimerWorker_t *self_;
  MilkTea_NonCopy(TimerWorkerWrapper)
  MilkTea_NonMoveAssign(TimerWorkerWrapper)
};

} // namespace TeaPot

#endif // ifndef LIB_TEAPOT_WRAPPER_TIMERWORKER_H_
