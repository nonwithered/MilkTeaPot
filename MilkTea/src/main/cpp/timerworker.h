#ifndef MILKTEA_TIMERWORKER_H_
#define MILKTEA_TIMERWORKER_H_

#include <milktea/defer.h>

#include <queue>
#include <mutex>
#include <condition_variable>
#include <tuple>

#include "timertask.h"

namespace MilkTea {

class TimerWorkerImpl final {
 public:
  using worker_type = std::shared_ptr<TimerWorkerImpl>;
  using task_type = TimerTaskImpl::task_type;
  using future_type = TimerTaskImpl::future_type;
  using clock_type = TimerTaskImpl::clock_type;
  using duration_type = TimerTaskImpl::duration_type;
  using time_point_type = TimerTaskImpl::time_point_type;
  using action_type = TimerTaskImpl::action_type;
  static worker_type Create(std::function<void(action_type)> call) {
    worker_type worker(new TimerWorkerImpl());
    if (worker->Start(call)) {
        return worker;
    }
    return worker_type(nullptr);
  }
  ~TimerWorkerImpl() {
    if (state_ == State::INIT) {
      return;
    }
    Shutdown();
    AwaitTermination();
  }
  enum class State {
    INIT,
    RUNNING,
    SHUTDOWN,
    STOP,
    TIDYING,
    TERMINATED
  };
  State state() const {
    return state_;
  }
  bool Start(std::function<void(action_type)> call) {
    if (!ChangeState(State::INIT, State::RUNNING)) {
      return false;
    }
    call([this]() -> void { Run(); });
    return true;
  }
  void AwaitTermination() {
    std::unique_lock guard(lock_);
    while (state_ != State::TERMINATED) {
      state_cond_.wait(guard);
    }
  }
  bool AwaitTermination(duration_type delay) {
    time_point_type target = CurrentTimePoint() + delay;
    std::unique_lock guard(lock_);
    while (true) {
      if (state_ == State::TERMINATED) {
        return true;
      }
      duration_type delta = target - CurrentTimePoint();
      if (delta > duration_type::zero()) {
        state_cond_.wait_for(guard, delta);
      } else {
        return false;
      }
    }
  }
  bool Shutdown() {
    return ChangeState(State::RUNNING, State::SHUTDOWN, [this]() -> void {
      tasks_cond_.notify_one();
    });
  }
  std::vector<task_type> ShutdownNow() {
    std::vector<task_type> vec;
    ChangeState(State::RUNNING, State::STOP, [this, &vec]() -> void {
      while (!tasks_.empty()) {
        vec.push_back(tasks_.top());
        tasks_.pop();
      }
      tasks_cond_.notify_one();
    });
    return vec;
  }
  future_type Post(action_type f, duration_type delay = duration_type::zero()) {
    if (delay < duration_type::zero()) {
      delay = duration_type::zero();
    }
    time_point_type target = CurrentTimePoint() + delay;
    future_type future = TimerFutureImpl::Create(target);
    std::lock_guard guard(lock_);
    if (state_ != State::RUNNING) {
      future->Cancel();
      return future;
    }
    bool wake = tasks_.empty() || *tasks_.top() - target > duration_type::zero();
    tasks_.push(TimerTaskImpl::Create(future, f));
    if (wake) {
      tasks_cond_.notify_one();
    }
    return future;
  }
 private:
  explicit TimerWorkerImpl(std::function<bool(std::exception *)> on_terminate = [](std::exception *) -> bool { return false; })
  : state_(State::INIT),
    on_terminate_(on_terminate) {}
  void Run() {
    try {
      while (true) {
        auto [b, f] = Take();
        if (!b) {
          break;
        }
        f();
      }
      TryTerminate(nullptr);
    } catch (std::exception &e) {
      if (!TryTerminate(&e)) {
        throw e;
      }
    }
  }
  bool TryTerminate(std::exception *e) {
    return ChangeState(State::TIDYING, [](State state) -> bool {
      switch (state) {
        case State::INIT:
          MilkTea_LogPrint(ERROR, TAG, "TryTerminate assert State::INIT");
          MilkTea_throw(Assertion, "TryTerminate assert State::INIT");
        case State::RUNNING:
          MilkTea_LogPrint(ERROR, TAG, "TryTerminate assert State::RUNNING");
          MilkTea_throw(Assertion, "TryTerminate assert State::RUNNING");
        case State::TIDYING:
        case State::TERMINATED:
          return false;
        case State::SHUTDOWN:
        case State::STOP:
          return true;
        default:
          MilkTea_LogPrint(ERROR, TAG, "TryTerminate assert default");
          MilkTea_throw(Assertion, "TryTerminate assert default");
      }
    }) && OnTerminate(e);
  }
  bool OnTerminate(std::exception *e) {
    Defer defer([this]() -> void {
      if (!ChangeState(State::TIDYING, State::TERMINATED, [this]() -> void {
        state_cond_.notify_all();
      })) {
        MilkTea_LogPrint(ERROR, TAG, "OnTerminate assert");
        MilkTea_throw(Assertion, "OnTerminate assert");
      }
    });
    return on_terminate_(e);
  }
  std::tuple<bool, action_type> Take() {
    std::unique_lock guard(lock_);
    while (true) {
      switch (state_) {
        case State::INIT:
          MilkTea_LogPrint(ERROR, TAG, "Take assert State::INIT");
          MilkTea_throw(Assertion, "Take assert State::INIT");
        case State::TIDYING:
          MilkTea_LogPrint(ERROR, TAG, "Take assert State::TIDYING");
          MilkTea_throw(Assertion, "Take assert State::TIDYING");
        case State::TERMINATED:
          MilkTea_LogPrint(ERROR, TAG, "Take assert State::TERMINATED");
          MilkTea_throw(Assertion, "Take assert State::TERMINATED");
      }
      if (!tasks_.empty()) {
        if (state_ == State::STOP) {
          MilkTea_LogPrint(ERROR, TAG, "Take assert State::STOP");
          MilkTea_throw(Assertion, "Take assert State::STOP");
        }
        duration_type delta = *tasks_.top() - CurrentTimePoint();
        if (delta > duration_type::zero()) {
          tasks_cond_.wait_for(guard, delta);
          continue;
        } else {
          task_type task = std::move(tasks_.top());
          tasks_.pop();
          return std::make_tuple(true, [task]() -> void {
            task->Run();
          });
        }
      } else {
        if (state_ == State::RUNNING) {
          tasks_cond_.wait(guard);
          continue;
        } else {
          return std::make_tuple(false, []() -> void {});
        }
      }
    }
  }
  bool ChangeState(State target, std::function<bool(State)> predicate = [](State) -> bool { return true; }, action_type action = []() -> void {}) {
    std::lock_guard guard(lock_);
    if (!predicate(state_)) {
      return false;
    }
    state_ = target;
    action();
    return true;
  }
  bool ChangeState(State expect, State target, action_type action = []() -> void {}) {
    return ChangeState(target, [expect](State state) -> bool { return state == expect; }, action);
  }
  static time_point_type CurrentTimePoint() {
    return std::chrono::time_point_cast<duration_type>(clock_type::now());
  }
  State state_;
  std::priority_queue<task_type, std::vector<task_type>, std::greater<task_type>> tasks_;
  std::recursive_mutex lock_;
  std::condition_variable_any state_cond_;
  std::condition_variable_any tasks_cond_;
  std::function<bool(std::exception *)> on_terminate_;
  static constexpr char TAG[] = "timer";
  MilkTea_NonCopy(TimerWorkerImpl)
  MilkTea_NonMove(TimerWorkerImpl)
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERWORKER_H_
