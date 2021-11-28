#ifndef TEAPOT_TIMERTASK_H_
#define TEAPOT_TIMERTASK_H_

#include <functional>
#include <exception>

#include <milktea.h>

#include "timerfuture.h"

namespace TeaPot {

using action_type = std::function<void()>;
class TimerTaskImpl;
using task_type = std::unique_ptr<TimerTaskImpl>;
using task_raw = task_type::element_type *;

class TimerTaskImpl final {
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
  static constexpr char TAG[] = "TeaPot#TimerTask";
};

} // namespace TeaPot

#endif // ifndef TEAPOT_TIMERTASK_H_
