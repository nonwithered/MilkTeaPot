#ifndef SOYMILK_FRAME_H_
#define SOYMILK_FRAME_H_

#include <vector>

#include <soymilk/common.h>

namespace SoyMilk {

namespace Codec {

class FrameEventImpl final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameEventImpl";
 public:
  explicit FrameEventImpl(uint16_t ntrk) : ntrk_(ntrk) {}
  uint16_t ntrk() const {
    return ntrk_;
  }
  const std::vector<MilkPowder::MessageMutableWrapper> &msg() const {
    return msg_;
  }
  void Append(MilkPowder::MessageMutableWrapper msg) {
    msg_.emplace_back(std::move(msg));
  }
 private:
  const uint16_t ntrk_;
  std::vector<MilkPowder::MessageMutableWrapper> msg_;
};

class FrameBufferImpl final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferImpl";
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  explicit FrameBufferImpl(duration_type time) : time_(time) {}
  duration_type time() const {
    return time_;
  }
  const std::vector<FrameEventImpl> &items() const {
    return items_;
  }
  FrameEventImpl &Append(FrameEventImpl item) {
    items_.emplace_back(std::move(item));
    return items_.back();
  }
  explicit operator FrameBufferWrapper() const {
    return reinterpret_cast<const FrameBufferWrapper::raw_type *>(this);
  }
 private:
  const duration_type time_;
  std::vector<FrameEventImpl> items_;
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef SOYMILK_FRAME_H_
