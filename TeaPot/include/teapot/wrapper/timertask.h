#ifndef LIB_TEAPOT_WRAPPER_TIMERTASK_H_
#define LIB_TEAPOT_WRAPPER_TIMERTASK_H_

#include <teapot/common.h>
#include <teapot/wrapper/timerfuture.h>

namespace TeaPot {

class TimerTaskWrapper final {
  static constexpr char TAG[] = "TeaPot::TimerTaskWrapper";
  friend class TimerWorkerWrapper;
 public:
  TimerTaskWrapper(TeaPot_TimerTask_t *self = nullptr) : self_(self) {}
  TimerTaskWrapper(TimerTaskWrapper &&another) : TimerTaskWrapper() {
    std::swap(self_, another.self_);
  }
  ~TimerTaskWrapper() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_panic(TeaPot_TimerTask_Destroy(self_));
    self_ = nullptr;
  }
  TimerFutureWrapper GetFuture() {
    TeaPot_TimerFuture_t *future = nullptr;
    MilkTea_panic(TeaPot_TimerTask_GetFuture(self_, &future));
    return future;
  }
  void Run() {
    MilkTea_panic(TeaPot_TimerTask_Run(self_));
  }
 private:
  TeaPot_TimerTask_t *self_;
  MilkTea_NonCopy(TimerTaskWrapper)
  MilkTea_NonMoveAssign(TimerTaskWrapper)
};

} // namespace TeaPot

#endif // ifndef LIB_TEAPOT_WRAPPER_TIMERTASK_H_
