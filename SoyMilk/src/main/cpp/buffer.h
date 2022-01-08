#ifndef SOYMILK_BUFFER_H_
#define SOYMILK_BUFFER_H_

#include <vector>

#include <soymilk/common.h>

#include "frame.h"
#include "sorter.h"

namespace SoyMilk {

namespace Codec {

class FrameBufferCursorImpl final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferCursorImpl";
  using iterator_type = std::list<FrameBufferImpl>::const_iterator;
 public:
  FrameBufferCursorImpl(iterator_type begin, iterator_type end)
  : tail_(end), iterator_(begin) {}
  const FrameBufferImpl *next() {
    if (iterator_ == tail_) {
      return nullptr;
    }
    const FrameBufferImpl &fbo = *iterator_;
    ++iterator_;
    return &fbo;
  }
 private:
   iterator_type tail_;
   iterator_type iterator_;
};

class FrameBufferQueueImpl final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferQueueImpl";
  using duration_type = TeaPot::TimerUnit::duration_type;
  using cursor_type = FrameBufferCursorImpl;
 public:
  FrameBufferQueueImpl()
  : queue_() {}
  duration_type Fill(MilkPowder::MidiConstWrapper midi) {
    auto &queue = queue_;
    if (!queue.empty()) {
      MilkTea_assert("fill but not empty");
    }

    auto time_begin = TeaPot::TimerUnit::Now();
    queue = FrameBufferSorterImpl::Sort(midi);
    auto time_end = TeaPot::TimerUnit::Now();
    auto time_duration = time_end - time_begin;
    MilkTea_logI("fill midi fbo queue during %" PRIi64 " ms", time_duration.count());

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
  std::list<FrameBufferImpl> queue_;
  MilkTea_NonCopy(FrameBufferQueueImpl)
  MilkTea_NonMove(FrameBufferQueueImpl)
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_BUFFER_H_
