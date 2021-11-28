#ifndef LIB_MILKTEA_TIMERWORKER_H_
#define LIB_MILKTEA_TIMERWORKER_H_

#ifdef __cplusplus
#include <milktea/timertask.h>
namespace MilkTea {

class TimerWorker final {
 public:
  enum class State {
    INIT,
    RUNNING,
    SHUTDOWN,
    STOP,
    TIDYING,
    TERMINATED,
    CLOSED
  };
  explicit TimerWorker(std::function<bool(std::exception *)> on_terminate) : TimerWorker() {
    std::function<bool(MilkTea_Exception_t, const char *)> do_terminate = [on_terminate](MilkTea_Exception_t type, const char *what) -> bool {
      Exception e = Exception::FromRawType(type, what);
      return on_terminate(&e);
    };
    MilkTea_TimerWorker_t *self = nullptr;
    MilkTea_panic(MilkTea_TimerWorker_Create(&self, ClosureToken<decltype(do_terminate)>::ToRawType(do_terminate), ClosureToken<decltype(do_terminate)>::Invoke));
    std::swap(self_, self);
  }
  TimerWorker(TimerWorker &&another) : TimerWorker() {
    std::swap(self_, another.self_);
  }
  ~TimerWorker() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_panic(MilkTea_TimerWorker_Destroy(self_));
    self_ = nullptr;
  }
  void Start() {
    MilkTea_panic(MilkTea_TimerWorker_Start(self_));
  }
  State GetState() {
    MilkTea_TimerWorker_State_t state = MilkTea_TimerWorker_State_CLOSED;
    MilkTea_panic(MilkTea_TimerWorker_GetState(self_, &state));
    return FromRawType(state);
  }
  TimerFuture Post(TimerUnit::duration_type delay, std::function<void()> action) {
    MilkTea_TimerFuture_t *future = nullptr;
    MilkTea_panic(MilkTea_TimerWorker_Post(self_, &future, delay.count(), ClosureToken<decltype(action)>::ToRawType(action), ClosureToken<decltype(action)>::Invoke));
    return TimerFuture(future);
  }
  bool Shutdown() {
    bool success = false;
    MilkTea_panic(MilkTea_TimerWorker_Shutdown(self_, &success));
    return success;
  }
  std::tuple<bool, std::vector<TimerTask>> ShutdownNow() {
    bool success = false;
    std::vector<TimerTask> vec;
    std::function<void(uint32_t, MilkTea_TimerTask_t **)> callback = [&success, &vec](uint32_t size, MilkTea_TimerTask_t ** tasks) -> void {
      success = true;
      for (uint32_t i = 0; i != size; ++i) {
        vec.push_back(TimerTask(tasks[i]));
      }
    };
    MilkTea_panic(MilkTea_TimerWorker_ShutdownNow(self_, &callback, ClosureToken<decltype(callback)>::Invoke));
    return std::make_tuple(success, std::move(vec));
  }
  bool AwaitTermination(TimerUnit::duration_type delay = TimerUnit::duration_type()) {
    bool success = false;
    MilkTea_panic(MilkTea_TimerWorker_AwaitTermination(self_, delay.count(), &success));
    return success;
  }
 private:
  static State FromRawType(MilkTea_TimerWorker_State_t state) {
    switch (state) {
      case MilkTea_TimerWorker_State_INIT: return State::INIT;
      case MilkTea_TimerWorker_State_RUNNING: return State::RUNNING;
      case MilkTea_TimerWorker_State_SHUTDOWN: return State::SHUTDOWN;
      case MilkTea_TimerWorker_State_STOP: return State::STOP;
      case MilkTea_TimerWorker_State_TIDYING: return State::TIDYING;
      case MilkTea_TimerWorker_State_TERMINATED: return State::TERMINATED;
      case MilkTea_TimerWorker_State_CLOSED: return State::CLOSED;
      default: MilkTea_assert("State assert");
    }
  }
  TimerWorker(MilkTea_TimerWorker_t *self = nullptr) : self_(self) {}
  MilkTea_TimerWorker_t *self_;
  MilkTea_NonCopy(TimerWorker)
  MilkTea_NonMoveAssign(TimerWorker)
  static constexpr char TAG[] = "MilkTea#TimerWorker";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERWORKER_H_
