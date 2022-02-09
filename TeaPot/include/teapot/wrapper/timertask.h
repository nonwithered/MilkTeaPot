#ifndef LIB_TEAPOT_WRAPPER_TIMERTASK_H_
#define LIB_TEAPOT_WRAPPER_TIMERTASK_H_

#include <teapot/common.h>
#include <teapot/wrapper/timerfuture.h>

namespace TeaPot {

class TimerTaskWrapper final {
  static constexpr char TAG[] = "TeaPot::TimerTaskWrapper";
  friend class TimerWorkerWrapper;
 public:
  TimerTaskWrapper(TeaPot_TimerTask_t *obj = nullptr) : obj_(obj) {}
  TimerTaskWrapper(TimerTaskWrapper &&another) : TimerTaskWrapper() {
    std::swap(obj_, another.obj_);
  }
  ~TimerTaskWrapper() {
    if (obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(TeaPot_TimerTask_Destroy, obj_);
    obj_ = nullptr;
  }
  TimerFutureWrapper GetFuture() {
    TeaPot_TimerFuture_t *future = nullptr;
    MilkTea_invoke_panic(TeaPot_TimerTask_GetFuture, obj_, &future);
    return future;
  }
  void Run() {
    MilkTea_invoke_panic(TeaPot_TimerTask_Run, obj_);
  }
 private:
  TeaPot_TimerTask_t *obj_;
  MilkTea_NonCopy(TimerTaskWrapper)
  MilkTea_NonMoveAssign(TimerTaskWrapper)
};

} // namespace TeaPot

#endif // ifndef LIB_TEAPOT_WRAPPER_TIMERTASK_H_
