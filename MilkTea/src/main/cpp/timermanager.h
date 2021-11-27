#ifndef MILKTEA_TIMERMANAGER_H_
#define MILKTEA_TIMERMANAGER_H_

#include <milktea.h>

#include <memory>
#include <functional>
#include <mutex>
#include <map>

#include "timertask.h"

namespace MilkTea {

class TimerWorkerImpl;

class TimerBinderImpl;

struct TimerWorkerIdentity;

#ifdef MilkTea_TimerManager_typedef
#undef MilkTea_TimerManager_typedef
#endif

#define MilkTea_TimerManager_typedef \
  using manager_type = MilkTea::TimerManagerImpl; \
  using worker_binder = MilkTea::TimerBinderImpl; \
  using worker_raw = MilkTea::TimerWorkerImpl; \
  using worker_type = std::shared_ptr<worker_raw>; \
  using worker_weak = std::weak_ptr<worker_raw>; \
  using worker_identity = MilkTea::TimerWorkerIdentity *;
class TimerManagerImpl final : public std::enable_shared_from_this<TimerManagerImpl> {
  friend class TimerBinderImpl;
  MilkTea_TimerManager_typedef
 public:
  static manager_type &Instance();
 private:
  TimerManagerImpl() = default;
  void Register(worker_identity identity, worker_type worker) {
    auto guard = Guard();
    auto it = workers_.find(identity);
    if (it != workers_.end()) {
      MilkTea_assert("register but exists");
    }
    workers_.insert(std::make_pair(identity, std::move(worker)));
  }
  void Unregister(worker_identity identity) {
    auto guard = Guard();
    auto it = workers_.find(identity);
    if (it == workers_.end()) {
      MilkTea_assert("unregister not found");
    }
    workers_.erase(it);
  }
  std::lock_guard<std::mutex> Guard() {
    return std::lock_guard(lock_);
  }
  std::map<worker_identity, worker_type> workers_;
  std::mutex lock_;
  MilkTea_NonCopy(TimerManagerImpl)
  MilkTea_NonMove(TimerManagerImpl)
  static constexpr char TAG[] = "MilkTea#TimerManager";
};

class TimerBinderImpl final {
  friend class TimerWorkerImpl;
  MilkTea_TimerManager_typedef
 public:
  TimerBinderImpl() = default;
 private:
  void WithGuard(std::function<void(worker_type)> callback) {
    auto guard = Guard();
    auto worker = worker_.lock();
    if (worker == nullptr) {
      return;
    }
    callback(std::move(worker));
  }
  void Bind(worker_type worker) {
    auto guard = Guard();
    worker_ = worker;
    manager_type::Instance().Register(Identity(), std::move(worker));
  }
  void Unbind(worker_type &&worker) {
    auto guard = Guard();
    manager_type::Instance().Unregister(Identity());
    worker.~shared_ptr();
  }
  worker_identity Identity() {
    return reinterpret_cast<worker_identity>(lock_.get());
  }
  std::lock_guard<std::mutex> Guard() {
    return std::lock_guard(*lock_);
  }
  worker_weak worker_;
  std::shared_ptr<std::mutex> lock_;
  static constexpr char TAG[] = "MilkTea#TimerHolder";
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERMANAGER_H_
