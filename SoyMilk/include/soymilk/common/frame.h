#ifndef LIB_SOYMILK_COMMON_FRAME_H_
#define LIB_SOYMILK_COMMON_FRAME_H_

#include <vector>
#include <functional>
#include <algorithm>

#include <soymilk/core.h>

namespace SoyMilk {

using tempo_type = MilkPowder::Mapping::Message::tempo_type;

class FrameBufferWrapper final {
  static constexpr char TAG[] = "SoyMilk::FrameBufferWrapper";
  using tempo_type = MilkPowder::Mapping::Message::tempo_type;
 public:
  using raw_type = SoyMilk_FrameBuffer_t;
  FrameBufferWrapper(raw_type *obj = nullptr) : obj_(obj) {}
  FrameBufferWrapper(const FrameBufferWrapper &another) : FrameBufferWrapper() {
    if (another.obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_Clone, another.obj_, &obj_);
  }
  FrameBufferWrapper(FrameBufferWrapper &&another) : FrameBufferWrapper(another.release()) {}
  ~FrameBufferWrapper() {
    auto obj = release();
    if (obj == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_Destroy, obj);
  }
  tempo_type GetTime() const {
    int64_t result = 0;
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_GetTime, get(), &result);
    return tempo_type(result);
  }
  void AllMessage(std::function<void(size_t, uint16_t, MilkPowder::MessageConstWrapper)> f) const {
    MilkTea_invoke_panic(SoyMilk_FrameBuffer_AllMessage, get(), MilkTea::FunctionAdapter::ToRawType<SoyMilk_FrameBuffer_Consumer_t>(f));
  }
  raw_type *get() const {
    return obj_;
  }
  raw_type *release() {
    return reset(nullptr);
  }
  raw_type *reset(raw_type *another) {
    std::swap(another, obj_);
    return another;
  }
 private:
  raw_type *obj_;
  MilkTea_NonAssign(FrameBufferWrapper)
};

} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_FRAME_H_
