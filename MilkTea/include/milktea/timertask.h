#ifndef LIB_MILKTEA_TIMERTASK_H_
#define LIB_MILKTEA_TIMERTASK_H_

#ifdef __cplusplus
#include <milktea/timerfuture.h>
#include <milktea/exception.h>
#include <milktea/logger.h>
#include <functional>
#include <chrono>
#include <exception>
namespace MilkTea {
class TimerTask final {
  friend class TimerWorker;
 private:
  using task_type = std::shared_ptr<TimerTask>;
  using future_type = TimerFuture::future_type;
  using clock_type = std::chrono::system_clock;
  using duration_type = std::chrono::milliseconds;
  using time_point_type = std::chrono::time_point<clock_type, duration_type>;
  using action_type = std::function<void()>;
 public:
  static task_type Create(future_type future, time_point_type time, action_type action) {
    return std::make_shared<TimerTask>(future, time, action);
  }
  TimerTask(future_type future, time_point_type time, action_type action) : future_(future), time_(time), action_(action) {}
 private:
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
  time_point_type time() const {
    return time_;
  }
  future_type future_;
  const time_point_type time_;
  const action_type action_;
  static constexpr char TAG[] = "timertask";
  MilkTea_NonCopy(TimerTask)
  MilkTea_NonMove(TimerTask)
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERTASK_H_
