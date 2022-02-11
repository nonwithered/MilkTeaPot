#include "frame.h"

namespace {

constexpr char TAG[] = "SoyMilk";

const SoyMilk::FrameBufferImpl &soymilk_cast(const SoyMilk_FrameBuffer_t *obj) {
  return *reinterpret_cast<const SoyMilk::FrameBufferImpl *>(obj);
}

SoyMilk::FrameBufferImpl &soymilk_cast(SoyMilk_FrameBuffer_t *obj) {
  return *reinterpret_cast<SoyMilk::FrameBufferImpl *>(obj);
}

const SoyMilk_FrameBuffer_t *soymilk_cast(const SoyMilk::FrameBufferImpl &obj) {
  return reinterpret_cast<const SoyMilk_FrameBuffer_t *>(&obj);
}

SoyMilk_FrameBuffer_t *soymilk_cast(SoyMilk::FrameBufferImpl &obj) {
  return reinterpret_cast<SoyMilk_FrameBuffer_t *>(&obj);
}

} // namespace

MilkTea_extern(SoyMilk_FrameBuffer_Clone, (const SoyMilk_FrameBuffer_t *obj, SoyMilk_FrameBuffer_t **another), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(another);
  *another = soymilk_cast(*new SoyMilk::FrameBufferImpl(soymilk_cast(obj)));
})

MilkTea_extern(SoyMilk_FrameBuffer_Destroy, (SoyMilk_FrameBuffer_t *obj), {
  MilkTea_nonnull(obj);
  delete &soymilk_cast(obj);
})

MilkTea_extern(SoyMilk_FrameBuffer_GetTime, (const SoyMilk_FrameBuffer_t *obj, int64_t *time), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(time);
  *time = soymilk_cast(obj).time().count();
})

MilkTea_extern(SoyMilk_FrameBuffer_AllMessage, (const SoyMilk_FrameBuffer_t *obj, SoyMilk_FrameBuffer_Consumer_t consumer), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(consumer.invoke_);
  soymilk_cast(obj).GetItems([consumer](auto index, auto ntrk, auto msg) {
    MilkTea_Function_Invoke(consumer, index, ntrk, msg.get());
  });
})
