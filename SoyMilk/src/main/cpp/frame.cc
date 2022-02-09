#include "frame.h"

namespace {

constexpr char TAG[] = "SoyMilk";

const SoyMilk::Codec::FrameEventImpl &soymilk_cast(const SoyMilk_FrameEvent_t *obj) {
  return *reinterpret_cast<const SoyMilk::Codec::FrameEventImpl *>(obj);
}

const SoyMilk_FrameEvent_t *soymilk_cast(const SoyMilk::Codec::FrameEventImpl &obj) {
  return reinterpret_cast<const SoyMilk_FrameEvent_t *>(&obj);
}

const SoyMilk::Codec::FrameBufferImpl &soymilk_cast(const SoyMilk_FrameBuffer_t *obj) {
  return *reinterpret_cast<const SoyMilk::Codec::FrameBufferImpl *>(obj);
}

SoyMilk::Codec::FrameBufferImpl &soymilk_cast(SoyMilk_FrameBuffer_t *obj) {
  return *reinterpret_cast<SoyMilk::Codec::FrameBufferImpl *>(obj);
}

SoyMilk_FrameBuffer_t *soymilk_cast(SoyMilk::Codec::FrameBufferImpl &obj) {
  return reinterpret_cast<SoyMilk_FrameBuffer_t *>(&obj);
}

} // namespace

MilkTea_extern(SoyMilk_FrameEvent_GetNtrk, (const SoyMilk_FrameEvent_t *obj, uint16_t *ntrk), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(ntrk);
  *ntrk = soymilk_cast(obj).ntrk();
})

MilkTea_extern(SoyMilk_FrameEvent_GetLen, (const SoyMilk_FrameEvent_t *obj, size_t *len), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(len);
  *len = soymilk_cast(obj).msg().size();
})

MilkTea_extern(SoyMilk_FrameEvent_GetMsg, (const SoyMilk_FrameEvent_t *obj, size_t idx, const MilkPowder_Message_t **msg), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(msg);
  size_t len = soymilk_cast(obj).msg().size();
  if (idx >= len) {
    MilkTea_throwf(LogicError, "out of bound -- idx: %zu, len: %zu", idx, len);
  }
  *msg = soymilk_cast(obj).msg()[idx].get();
})

MilkTea_extern(SoyMilk_FrameBuffer_Clone, (SoyMilk_FrameBuffer_t **obj, const SoyMilk_FrameBuffer_t *another), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(another);
  *obj = soymilk_cast(*new SoyMilk::Codec::FrameBufferImpl(soymilk_cast(another)));
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

MilkTea_extern(SoyMilk_FrameBuffer_GetLen, (const SoyMilk_FrameBuffer_t *obj, size_t *len), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(len);
  *len = soymilk_cast(obj).items().size();
})

MilkTea_extern(SoyMilk_FrameBuffer_GetItem, (const SoyMilk_FrameBuffer_t *obj, size_t idx, const SoyMilk_FrameEvent_t **item), {
  MilkTea_nonnull(obj);
  MilkTea_nonnull(item);
  size_t len = soymilk_cast(obj).items().size();
  if (idx >= len) {
    MilkTea_throwf(LogicError, "out of bound -- idx: %zu, len: %zu", idx, len);
  }
  *item = soymilk_cast(soymilk_cast(obj).items()[idx]);
})
