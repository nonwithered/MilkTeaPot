#ifndef LIB_MILKTEA_TIMERTASK_H_
#define LIB_MILKTEA_TIMERTASK_H_

#ifdef __cplusplus
#include <milktea/timerfuture.h>
namespace MilkTea {

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
    MilkTea_panic(MilkTea_TimerTask_Destroy(self_));
    self_ = nullptr;
  }
  TimerFuture GetFuture() {
    MilkTea_TimerFuture_t *future = nullptr;
    MilkTea_panic(MilkTea_TimerTask_GetFuture(self_, &future));
    return TimerFuture(future);
  }
  void Run() {
    MilkTea_panic(MilkTea_TimerTask_Run(self_));
  }
 private:
  TimerTask(MilkTea_TimerTask_t *self = nullptr) : self_(self) {}
  MilkTea_TimerTask_t *self_;
  MilkTea_NonCopy(TimerTask)
  MilkTea_NonMoveAssign(TimerTask)
  static constexpr char TAG[] = "MilkTea#TimerTask";
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERTASK_H_
