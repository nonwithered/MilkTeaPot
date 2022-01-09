#ifndef LIB_TEAPOT_WRAPPER_TIMERFUTURE_H_
#define LIB_TEAPOT_WRAPPER_TIMERFUTURE_H_

#include <teapot/common.h>

namespace TeaPot {

class TimerFutureWrapper final {
  static constexpr char TAG[] = "TeaPot::TimerFutureWrapper";
  friend class TimerTaskWrapper;
  friend class TimerWorkerWrapper;
  using raw_type = TeaPot_TimerFuture_t;
  using State = TeaPot::TimerFuture::State;
  using duration_type = TeaPot::TimerUnit::duration_type;
  using time_point_type = TeaPot::TimerUnit::time_point_type;
 public:
  TimerFutureWrapper(raw_type *self = nullptr) : self_(self) {}
  TimerFutureWrapper(TimerFutureWrapper &&another) : TimerFutureWrapper() {
    std::swap(self_, another.self_);
  }
  ~TimerFutureWrapper() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(TeaPot_TimerFuture_Destroy, self_);
    self_ = nullptr;
  }
  bool Cancel() {
    bool success = false;
    MilkTea_invoke_panic(TeaPot_TimerFuture_Cancel, self_, &success);
    return success;
  }
  State GetState() {
    TeaPot_TimerFuture_State_t state = TeaPot_TimerFuture_State_EXCEPTIONAL;
    MilkTea_invoke_panic(TeaPot_TimerFuture_GetState, self_, &state);
    return TeaPot::TimerFuture::FromRawType(state);
  }
  time_point_type GetTime() {
    int64_t time = 0;
    MilkTea_invoke_panic(TeaPot_TimerFuture_GetTime, self_, &time);
    return time_point_type(duration_type(time));
  }
  raw_type *release() {
    raw_type *self = self_;
    self_ = nullptr;
    return self;
  }
  raw_type *get() const {
    return self_;
  }
 private:
  raw_type *self_;
  MilkTea_NonCopy(TimerFutureWrapper)
  MilkTea_NonMoveAssign(TimerFutureWrapper)
};

} // namespace TeaPot

#endif // ifndef LIB_TEAPOT_WRAPPER_TIMERFUTURE_H_
