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
 public:
  FrameBufferQueueImpl()
  : queue_() {}
  duration_type Fill(MilkPowder::MidiConstWrapper midi) {
    if (!queue_.empty()) {
      MilkTea_assert("fill but not empty");
    }
    if (queue_.empty()) {
      MilkTea_logW("fill but empty");
      return duration_type(-1);
    }
    return queue_.back().time();
  }
  void Sweep() {
    queue_.clear();
  }
 private:
  std::vector<FrameBufferImpl> queue_;
  MilkTea_NonCopy(FrameBufferQueueImpl)
  MilkTea_NonMove(FrameBufferQueueImpl)
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_DECODE_H_
