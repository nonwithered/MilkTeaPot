#ifndef LIB_TEAPOT_TIMERTASK_H_
#define LIB_TEAPOT_TIMERTASK_H_

#ifdef __cplusplus

#include <teapot/core.h>
#include <teapot/timerfuture.h>

namespace TeaPot {

class TimerTask final {
  friend class TimerWorker;
 public:
  TimerTask(TimerTask &&another) : TimerTask() {
    std::swap(self_, another.self_);
  }
  ~TimerTask() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_panic(TeaPot_TimerTask_Destroy(self_));
    self_ = nullptr;
  }
  TimerFuture GetFuture() {
    TeaPot_TimerFuture_t *future = nullptr;
    MilkTea_panic(TeaPot_TimerTask_GetFuture(self_, &future));
    return TimerFuture(future);
  }
  void Run() {
    MilkTea_panic(TeaPot_TimerTask_Run(self_));
  }
 private:
  TimerTask(TeaPot_TimerTask_t *self = nullptr) : self_(self) {}
  TeaPot_TimerTask_t *self_;
  MilkTea_NonCopy(TimerTask)
  MilkTea_NonMoveAssign(TimerTask)
  static constexpr char TAG[] = "TeaPot#TimerTask";
};

} // namespace TeaPot
#endif // ifdef __cplusplus

#endif // ifndef LIB_TEAPOT_TIMERTASK_H_
