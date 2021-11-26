#ifndef MILKTEA_TIMERTASK_H_
#define MILKTEA_TIMERTASK_H_

#include <functional>
#include <exception>

#include <milktea.h>

#include "timerfuture.h"

namespace MilkTea {

#ifdef MilkTea_TimerTask_typedef
#undef MilkTea_TimerTask_typedef
#endif

#define MilkTea_TimerTask_typedef \
  using task_raw = MilkTea::TimerTaskImpl *; \
  using task_type = std::unique_ptr<MilkTea::TimerTaskImpl>; \
  using action_type = std::function<void()>;

class TimerTaskImpl final {
  MilkTea_TimerFuture_typedef
  MilkTea_TimerTask_typedef
 public:
  static task_type Make(future_type future, action_type action) {
    return task_type(new TimerTaskImpl(future, action));
  }
  bool operator>(const TimerTaskImpl &another) const {
    return future_->time() > another.future_->time();
  }
  duration_type operator-(time_point_type time) const {
    return future_->time() - time;
  }
  future_type future() {
    return future_;
  }
  void Run() {
    if (!future_->ChangeState(TimerFutureImpl::State::SCHEDULED, TimerFutureImpl::State::EXECUTED)) {
        MilkTea_assert("Run -- assert SCHEDULED -> EXECUTED");
    }
    try {
      action_();
      if (!future_->ChangeState(TimerFutureImpl::State::EXECUTED, TimerFutureImpl::State::NORMAL)) {
        MilkTea_assert("Run -- assert EXECUTED -> NORMAL");
      }
    } catch (std::exception &e) {
      if (!future_->ChangeState(TimerFutureImpl::State::EXECUTED, TimerFutureImpl::State::EXCEPTIONAL)) {
        MilkTea_assert("Run -- assert EXECUTED -> EXCEPTIONAL");
      }
      throw e;
    }
  }
 private:
  TimerTaskImpl(future_type future, action_type action) : future_(future), action_(action) {}
  future_type future_;
  const action_type action_;
  MilkTea_NonCopy(TimerTaskImpl)
  MilkTea_NonMove(TimerTaskImpl)
  static constexpr char TAG[] = "MilkTea#TimerTask";
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERTASK_H_
