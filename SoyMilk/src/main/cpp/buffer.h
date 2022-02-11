#ifndef SOYMILK_BUFFER_H_
#define SOYMILK_BUFFER_H_

#include <vector>

#include <soymilk/common.h>

#include "frame.h"
#include "sorter.h"

namespace SoyMilk {

class FrameBufferCursorImpl final {
  static constexpr char TAG[] = "SoyMilk::FrameBufferCursorImpl";
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
      MilkTea_assert("next but tail");
    }
    ++iterator_;
  }
 private:
   iterator_type tail_;
   iterator_type iterator_;
};

class FrameBufferQueueImpl final {
  static constexpr char TAG[] = "SoyMilk::FrameBufferQueueImpl";
  using duration_type = TeaPot::TimerUnit::duration_type;
  using queue_type = FrameBufferSorterImpl::queue_type;
  using cursor_type = FrameBufferCursorImpl;
 public:
  FrameBufferQueueImpl()
  : queue_() {}
  duration_type Fill(std::vector<MilkPowder::MidiConstWrapper> vec) {
    auto &queue = queue_;
    if (!queue.empty()) {
      MilkTea_assert("fill but not empty");
    }

    auto time_begin = TeaPot::TimerUnit::Now();
    queue = FrameBufferSorterImpl::Generate(std::move(vec));
    auto time_end = TeaPot::TimerUnit::Now();
    auto time_duration = time_end - time_begin;
    MilkTea_logI("fill midi fbo queue during %" PRId64 " us", time_duration.count());

    if (queue.empty()) {
      MilkTea_logW("fill but empty");
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
  MilkTea_NonCopy(FrameBufferQueueImpl)
  MilkTea_NonMove(FrameBufferQueueImpl)
};

} // namespace SoyMilk

#endif // ifndef SOYMILK_BUFFER_H_
