#ifndef LIB_MILKTEA_TIMERTASK_H_
#define LIB_MILKTEA_TIMERTASK_H_

#ifdef __cplusplus
#include <milktea/exception.h>
#include <milktea/logger.h>
#include <milktea/timerfuture.h>
#include <functional>
#include <exception>
namespace MilkTea {

class TimerTask final {
 public:
  using task_type = std::shared_ptr<TimerTask>;
  using future_type = TimerFuture::future_type;
  using clock_type = TimerFuture::clock_type;
  using duration_type = TimerFuture::duration_type;
  using time_point_type = TimerFuture::time_point_type;
  using action_type = std::function<void()>;
  static task_type Create(future_type future, action_type action) {
    return task_type(new TimerTask(future, action));
  }
  bool operator>(const TimerTask &another) const {
    return future_->time() > another.future_->time();
  }
  duration_type operator-(time_point_type time) const {
    return future_->time() - time;
  }
  future_type future() {
    return future_;
  }
  void Run() {
    if (!future_->ChangeState(TimerFuture::State::SCHEDULED, TimerFuture::State::EXECUTED)) {
        return;
    }
    try {
      action_();
      if (!future_->ChangeState(TimerFuture::State::EXECUTED, TimerFuture::State::NORMAL)) {
        MilkTea_LogPrint(ERROR, TAG, "Run assert NORMAL");
        MilkTea_throw(Assertion, "Run assert NORMAL");
      }
    } catch (std::exception &e) {
      if (!future_->ChangeState(TimerFuture::State::EXECUTED, TimerFuture::State::EXCEPTIONAL)) {
          MilkTea_LogPrint(ERROR, TAG, "Run assert EXCEPTIONAL");
          MilkTea_throw(Assertion, "Run assert EXCEPTIONAL");
      }
      throw e;
    }
  }
 private:
  TimerTask(future_type future, action_type action) : future_(future), action_(action) {}
  future_type future_;
  const action_type action_;
  static constexpr char TAG[] = "timertask";
  MilkTea_NonCopy(TimerTask)
  MilkTea_NonMove(TimerTask)
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERTASK_H_
