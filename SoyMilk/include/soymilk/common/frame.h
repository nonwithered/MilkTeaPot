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
  FrameEventWrapper(const raw_type *another) : obj_(another) {}
  uint16_t GetNtrk() const {
    uint16_t result = 0;
    MilkTea_invoke_panic(SoyMilk_FrameEvent_GetNtrk, get(), &result);
    return result;
  }
  size_t GetLen() const {
    size_t result = 0;
    MilkTea_invoke_panic(SoyMilk_FrameEvent_GetLen, get(), &result);
    return result;
  }
  MilkPowder::MessageConstWrapper GetMsg(size_t idx) const {
    const MilkPowder::Mapping::Message::raw_type *result = nullptr;
    MilkTea_invoke_panic(SoyMilk_FrameEvent_GetMsg, get(), idx, &result);
    return result;
  }
  const raw_type *get() const {
    return obj_;
  }
 private:
  const raw_type *const obj_;
};

class FrameBufferWrapper final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferWrapper";
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  using raw_type = SoyMilk_FrameBuffer_t;
  FrameBufferWrapper(const raw_type *obj) : obj_(obj) {}
  duration_type GetTime() const {
    int64_t result = 0;
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_GetTime, get(), &result);
    return duration_type(result);
  }
  size_t GetLen() const {
    size_t result = 0;
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_GetLen, get(), &result);
    return result;
  }
  FrameEventWrapper GetItem(size_t idx) const {
    const FrameEventWrapper::raw_type *result = nullptr;
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_GetItem, get(), idx, &result);
    return result;
  }
  const raw_type *get() const {
    return obj_;
  }
 private:
  const raw_type *const obj_;
};

class FrameBufferHolder final {
  static constexpr char TAG[] = "SoyMilk::Codec::FrameBufferHolder";
  using raw_type = FrameBufferWrapper::raw_type;
 public:
  FrameBufferHolder(raw_type *obj = nullptr) : obj_(obj) {}
  FrameBufferHolder(FrameBufferHolder &&another) : FrameBufferHolder() {
    std::swap(obj_, another.obj_);
  }
  explicit FrameBufferHolder(FrameBufferWrapper another) : FrameBufferHolder() {
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_Clone, &obj_, another.get());
  }
  ~FrameBufferHolder() {
    if (obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_Destroy, obj_);
    obj_ = nullptr;
  }
  operator FrameBufferWrapper() const {
    return get();
  }
  const raw_type *get() const {
    return obj_;
  }
  raw_type *get() {
    return obj_;
  }
  raw_type *release() {
    raw_type *result = obj_;
    obj_ = nullptr;
    return result;
  }
 private:
  raw_type *obj_;
  MilkTea_NonCopy(FrameBufferHolder)
  MilkTea_NonMoveAssign(FrameBufferHolder)
};

} // namespace Codec
} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_FRAME_H_
