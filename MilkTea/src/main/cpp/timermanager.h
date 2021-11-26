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

class TimerHolderImpl;

#ifdef MilkTea_TimerManager_typedef
#undef MilkTea_TimerManager_typedef
#endif

#define MilkTea_TimerManager_typedef \
  using manager_raw = MilkTea::TimerManagerImpl; \
  using manager_type = std::shared_ptr<manager_raw>; \
  using manager_weak = std::weak_ptr<manager_raw>; \
  using holder_id = std::mutex *; \
  using worker_holder = MilkTea::TimerHolderImpl; \
  using worker_raw = MilkTea::TimerWorkerImpl; \
  using worker_type = std::shared_ptr<worker_raw>; \
  using worker_weak = std::weak_ptr<worker_raw>;

class TimerManagerImpl final : public std::enable_shared_from_this<TimerManagerImpl> {
  friend class TimerWorkerImpl;
  friend class TimerHolderImpl;
  MilkTea_TimerManager_typedef
 public:
  TimerManagerImpl() = default;
 private:
  holder_id Register(worker_weak worker) {
    auto worker_ = worker.lock();
    if (worker_ == nullptr) {
      MilkTea_assert("register but nullptr");
    }
    auto id = std::make_unique<std::mutex>();
    std::lock_guard guard(lock_);
    auto it = workers_.find(id.get());
    if (it != workers_.end()) {
      MilkTea_assert("register but exists");
    }
    workers_.insert(std::make_pair(id.get(), std::move(worker_)));
    return id.release();
  }
  void Unregister(holder_id id) {
    std::lock_guard guard(lock_);
    auto it = workers_.find(id);
    if (it == workers_.end()) {
      MilkTea_assert("unregister not found");
    }
    workers_.erase(it);
  }
  std::map<holder_id, worker_type> workers_;
  std::mutex lock_;
  MilkTea_NonCopy(TimerManagerImpl)
  MilkTea_NonMove(TimerManagerImpl)
  static constexpr char TAG[] = "MilkTea#TimerManager";
};

class TimerHolderImpl final {
  friend class TimerWorkerImpl;
  MilkTea_TimerManager_typedef
 public:
  TimerHolderImpl(manager_type manager = nullptr, std::mutex *lock = new std::mutex())
  : manager_(manager),
    lock_(lock) {}
 private:
  void Obtain(std::function<void()> callback) {
    std::lock_guard guard(*lock_);
    callback();
  }
  void Close(worker_type &&worker) {
    auto manager = manager_.lock();
    if (manager == nullptr) {
      MilkTea_assert("Close but manager is nullptr");
    }
    std::lock_guard guard(*lock_);
    manager->Unregister(lock_.get());
    worker.~shared_ptr();
  }
  manager_weak manager_;
  std::shared_ptr<std::mutex> lock_;
  static constexpr char TAG[] = "MilkTea#TimerHolder";
};

} // namespace MilkTea

#endif // ifndef MILKTEA_TIMERMANAGER_H_
