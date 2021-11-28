#ifndef LIB_TEAPOT_TIMERWORKER_H_
#define LIB_TEAPOT_TIMERWORKER_H_

#ifdef __cplusplus

#include <teapot/core.h>
#include <teapot/timertask.h>

namespace TeaPot {

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
  TimerWorker(TimerWorker &&another) : TimerWorker() {
    std::swap(self_, another.self_);
  }
  ~TimerWorker() {
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
    return FromRawType(state);
  }
  TimerFuture Post(duration_type delay, std::function<void()> action) {
    TeaPot_TimerFuture_t *future = nullptr;
    MilkTea_panic(TeaPot_TimerWorker_Post(self_, &future, delay.count(), MilkTea::ClosureToken<decltype(action)>::ToRawType(action), MilkTea::ClosureToken<decltype(action)>::Invoke));
    return TimerFuture(future);
  }
  bool Shutdown() {
    bool success = false;
    MilkTea_panic(TeaPot_TimerWorker_Shutdown(self_, &success));
    return success;
  }
  std::tuple<bool, std::vector<TimerTask>> ShutdownNow() {
    bool success = false;
    std::vector<TimerTask> vec;
    std::function<void(uint32_t, TeaPot_TimerTask_t **)> callback = [&success, &vec](uint32_t size, TeaPot_TimerTask_t ** tasks) -> void {
      success = true;
      for (uint32_t i = 0; i != size; ++i) {
        vec.push_back(TimerTask(tasks[i]));
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
  static State FromRawType(TeaPot_TimerWorker_State_t state) {
    switch (state) {
      case TeaPot_TimerWorker_State_INIT: return State::INIT;
      case TeaPot_TimerWorker_State_RUNNING: return State::RUNNING;
      case TeaPot_TimerWorker_State_SHUTDOWN: return State::SHUTDOWN;
      case TeaPot_TimerWorker_State_STOP: return State::STOP;
      case TeaPot_TimerWorker_State_TIDYING: return State::TIDYING;
      case TeaPot_TimerWorker_State_TERMINATED: return State::TERMINATED;
      case TeaPot_TimerWorker_State_CLOSED: return State::CLOSED;
      default: MilkTea_assert("State FromRawType assert");
    }
  }
  static TeaPot_TimerWorker_State_t ToRawType(State state) {
    switch (state) {
      case State::INIT: return TeaPot_TimerWorker_State_INIT;
      case State::RUNNING: return TeaPot_TimerWorker_State_RUNNING;
      case State::SHUTDOWN: return TeaPot_TimerWorker_State_SHUTDOWN;
      case State::STOP: return TeaPot_TimerWorker_State_STOP;
      case State::TIDYING: return TeaPot_TimerWorker_State_TIDYING;
      case State::TERMINATED: return TeaPot_TimerWorker_State_TERMINATED;
      default: MilkTea_assert("State ToRawType assert");
    }
  }
 private:
  TimerWorker(TeaPot_TimerWorker_t *self = nullptr) : self_(self) {}
  TeaPot_TimerWorker_t *self_;
  MilkTea_NonCopy(TimerWorker)
  MilkTea_NonMoveAssign(TimerWorker)
  static constexpr char TAG[] = "TeaPot#TimerWorker";
};

} // namespace TeaPot
#endif // ifdef __cplusplus

#endif // ifndef LIB_TEAPOT_TIMERWORKER_H_
