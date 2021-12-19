#ifndef LIB_SOYMILK_COMMON_FRAME_H_
#define LIB_SOYMILK_COMMON_FRAME_H_

#include <vector>
#include <functional>
#include <algorithm>

#include <soymilk/core.h>

namespace SoyMilk {

namespace Codec {

class FrameEventWrapper final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameEvent";
 public:
  using raw_type = SoyMilk_FrameEvent_t;
  FrameEventWrapper(const raw_type *another) : self_(another) {}
  uint16_t GetNtrk() const {
    uint16_t result = 0;
    MilkTea_panic(SoyMilk_FrameEvent_GetNtrk(get(), &result));
    return result;
  }
  size_t GetLen() const {
    size_t result = 0;
    MilkTea_panic(SoyMilk_FrameEvent_GetLen(get(), &result));
    return result;
  }
  MilkPowder::MessageConstWrapper GetMsg(size_t idx) const {
    const MilkPowder::Mapping::Message::raw_type *result = nullptr;
    MilkTea_panic(SoyMilk_FrameEvent_GetMsg(get(), idx, &result));
    return result;
  }
  const raw_type *get() const {
    return self_;
  }
 private:
  const raw_type *const self_;
};

class FrameBufferWrapper final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferWrapper";
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  using raw_type = SoyMilk_FrameBuffer_t;
  FrameBufferWrapper(const raw_type *self) : self_(self) {}
  duration_type GetTime() const {
    int64_t result = 0;
    MilkTea_panic(SoyMilk_FrameBuffer_GetTime(get(), &result));
    return duration_type(result);
  }
  size_t GetLen() const {
    size_t result = 0;
    MilkTea_panic(SoyMilk_FrameBuffer_GetLen(get(), &result));
    return result;
  }
  FrameEventWrapper GetItem(size_t idx) const {
    const FrameEventWrapper::raw_type *result = nullptr;
    MilkTea_panic(SoyMilk_FrameBuffer_GetItem(get(), idx, &result));
    return result;
  }
  const raw_type *get() const {
    return self_;
  }
 private:
  const raw_type *const self_;
};

class FrameBufferHolder final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferHolder";
  using raw_type = FrameBufferWrapper::raw_type;
 public:
  FrameBufferHolder(raw_type *self = nullptr) : self_(self) {}
  FrameBufferHolder(FrameBufferHolder &&another) : FrameBufferHolder() {
    std::swap(self_, another.self_);
  }
  explicit FrameBufferHolder(FrameBufferWrapper another) : FrameBufferHolder() {
    MilkTea_panic(SoyMilk_FrameBuffer_Clone(&self_, another.get()));
  }
  ~FrameBufferHolder() {
    if (self_ == nullptr) {
      return;
    }
    MilkTea_panic(SoyMilk_FrameBuffer_Destroy(self_));
    self_ = nullptr;
  }
  operator FrameBufferWrapper() const {
    return get();
  }
  const raw_type *get() const {
    return self_;
  }
  raw_type *get() {
    return self_;
  }
  raw_type *release() {
    raw_type *result = self_;
    self_ = nullptr;
    return result;
  }
 private:
  raw_type *self_;
  MilkTea_NonCopy(FrameBufferHolder)
  MilkTea_NonMoveAssign(FrameBufferHolder)
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_FRAME_H_
