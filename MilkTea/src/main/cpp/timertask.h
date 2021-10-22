#ifndef MILKTEA_TIMERTASK_H_
#define MILKTEA_TIMERTASK_H_

#include <milktea/exception.h>
#include <milktea/logger.h>

#include <functional>
#include <exception>

#include "timerfuture.h"

namespace MilkTea {

class TimerTaskImpl final {
 public:
  using task_type = std::shared_ptr<TimerTaskImpl>;
  using future_type = TimerFutureImpl::future_type;
  using clock_type = TimerFutureImpl::clock_type;
  using duration_type = TimerFutureImpl::duration_type;
  using time_point_type = TimerFutureImpl::time_point_type;
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
        return;
    }
    try {
      action_();
      if (!future_->ChangeState(TimerFutureImpl::State::EXECUTED, TimerFutureImpl::State::NORMAL)) {
        MilkTea_LogPrint(ERROR, TAG, "Run assert NORMAL");
        MilkTea_throw(Assertion, "Run assert NORMAL");
      }
    } catch (std::exception &e) {
      if (!future_->ChangeState(TimerFutureImpl::State::EXECUTED, TimerFutureImpl::State::EXCEPTIONAL)) {
          MilkTea_LogPrint(ERROR, TAG, "Run assert EXCEPTIONAL");
          MilkTea_throw(Assertion, "Run assert EXCEPTIONAL");
      }
      throw e;
    }
  }
 private:
  TimerTaskImpl(future_type future, action_type action) : future_(future), action_(action) {}
  future_type future_;
  const action_type action_;
  static constexpr char TAG[] = "timertask";
  MilkTea_NonCopy(TimerTaskImpl)
  MilkTea_NonMove(TimerTaskImpl)
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERTASK_H_
