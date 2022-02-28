#ifndef SOYMILK_BUFFER_H_
#define SOYMILK_BUFFER_H_

#include <vector>

#include <tea.h>

#include <soymilk.h>

#include "frame.h"
#include "sorter.h"

namespace SoyMilk {

namespace internal {

class FrameBufferCursorImpl final{
  using queue_type = FrameBufferSorterImpl::queue_type;
  using iterator_type = queue_type::const_iterator;
 public:
  FrameBufferCursorImpl(iterator_type begin, iterator_type end)
  : tail_(end), iterator_(begin) {}
  const FrameBufferImpl *operator*() const {
    if (iterator_ == tail_) {
      return nullptr;
    }
    return &*iterator_;
  }
  void operator++() {
    if (iterator_ == tail_) {
      return;
    }
    ++iterator_;
  }
 private:
   iterator_type tail_;
   iterator_type iterator_;
};

class FrameBufferQueueImpl final : tea::with_log<FrameBufferQueueImpl>, tea::remove_move {
  using queue_type = FrameBufferSorterImpl::queue_type;
  using cursor_type = FrameBufferCursorImpl;
 public:
  static constexpr char TAG[] = "FrameBufferQueue";
  FrameBufferQueueImpl()
  : queue_() {}
  duration_type Fill(std::vector<const MilkPowder::Midi *> vec) {
    auto &queue = queue_;
    if (!queue.empty()) {
      log<W>("fill but not empty");
      return duration_type::zero();
    }
    auto time_begin = Now();
    queue = FrameBufferSorterImpl::Generate(std::move(vec));
    auto time_end = Now();
    auto time_duration = time_end - time_begin;
    log<I>("fill midi fbo queue during %" PRId64 " us", time_duration.count());

    if (queue.empty()) {
      log<W>("fill but empty");
      return duration_type(-1);
    }
    return queue_.back().time();
  }
  void Sweep() {
    auto &queue = queue_;
    if (queue.empty()) {
      return;
    }
    queue.clear();
  }
  cursor_type Cursor() const {
    auto &queue = queue_;
    if (queue.empty()) {
      return cursor_type(queue.end(), queue.end());
    }
    return cursor_type(queue.begin(), queue.end());
  }
 private:
  queue_type queue_;
};

} // namespace internal
} // namespace SoyMilk

#endif // ifndef SOYMILK_BUFFER_H_
