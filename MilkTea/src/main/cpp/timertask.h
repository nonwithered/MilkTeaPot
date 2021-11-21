#ifndef MILKTEA_TIMERTASK_H_
#define MILKTEA_TIMERTASK_H_

#include <functional>
#include <exception>

#include <milktea.h>

#include "timerfuture.h"

namespace MilkTea {

class TimerTaskImpl final {
  using future_type = TimerFutureImpl::future_type;
  using clock_type = TimerFutureImpl::clock_type;
  using duration_type = TimerFutureImpl::duration_type;
  using time_point_type = TimerFutureImpl::time_point_type;
 public:
  using task_raw = TimerTaskImpl *;
  using task_type = std::unique_ptr<TimerTaskImpl>;
  using action_type = std::function<void()>;
  static task_type Create(future_type future, action_type action) {
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
