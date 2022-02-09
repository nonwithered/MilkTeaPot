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
  TimerFutureWrapper(raw_type *obj = nullptr) : obj_(obj) {}
  TimerFutureWrapper(TimerFutureWrapper &&another) : TimerFutureWrapper() {
    std::swap(obj_, another.obj_);
  }
  ~TimerFutureWrapper() {
    if (obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(TeaPot_TimerFuture_Destroy, obj_);
    obj_ = nullptr;
  }
  bool Cancel() {
    bool success = false;
    MilkTea_invoke_panic(TeaPot_TimerFuture_Cancel, obj_, &success);
    return success;
  }
  State GetState() {
    TeaPot_TimerFuture_State_t state = TeaPot_TimerFuture_State_EXCEPTIONAL;
    MilkTea_invoke_panic(TeaPot_TimerFuture_GetState, obj_, &state);
    return TeaPot::TimerFuture::FromRawType(state);
  }
  time_point_type GetTime() {
    int64_t time = 0;
    MilkTea_invoke_panic(TeaPot_TimerFuture_GetTime, obj_, &time);
    return time_point_type(duration_type(time));
  }
  raw_type *release() {
    raw_type *obj = obj_;
    obj_ = nullptr;
    return obj;
  }
  raw_type *get() const {
    return obj_;
  }
 private:
  raw_type *obj_;
  MilkTea_NonCopy(TimerFutureWrapper)
  MilkTea_NonMoveAssign(TimerFutureWrapper)
};

} // namespace TeaPot

#endif // ifndef LIB_TEAPOT_WRAPPER_TIMERFUTURE_H_
