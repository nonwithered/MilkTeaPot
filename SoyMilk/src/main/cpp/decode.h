#ifndef SOYMILK_DECODE_H_
#define SOYMILK_DECODE_H_

#include <atomic>

#include <soymilk/common.h>

#include "frame.h"

namespace SoyMilk {

namespace Codec {

class FrameBufferQueueImpl final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferQueueImpl";
  using duration_type = TeaPot::TimerUnit::duration_type;
  using time_point_type = TeaPot::TimerUnit::time_point_type;
 public:
  FrameBufferQueueImpl()
  : queue_(),
    tag_(time_point_type()) {}
  void Fill(MilkPowder::MidiConstWrapper midi) {
    if (!queue_.empty()) {
      MilkTea_assert("fill but not empty");
    }
    // todo
  }
  void Sweep() {
    queue_.clear();
  }
  duration_type Length() const {
    if (queue_.empty()) {
      MilkTea_assert("want to know tail but empty now");
    }
    return queue_.back().time();
  }
  time_point_type tag() const {
    return tag_;
  }
  void tag(time_point_type time) {
    tag_ = time;
  }
 private:
  std::vector<FrameBufferImpl> queue_;
  std::atomic<time_point_type> tag_;
  MilkTea_NonCopy(FrameBufferQueueImpl)
  MilkTea_NonMove(FrameBufferQueueImpl)
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_DECODE_H_
