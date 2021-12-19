#include "frame.h"

namespace {

constexpr char TAG[] = "SoyMilk";

const SoyMilk::Codec::FrameEventImpl &soymilk_cast(const SoyMilk_FrameEvent_t *self) {
  return *reinterpret_cast<const SoyMilk::Codec::FrameEventImpl *>(self);
}

const SoyMilk_FrameEvent_t *soymilk_cast(const SoyMilk::Codec::FrameEventImpl &self) {
  return reinterpret_cast<const SoyMilk_FrameEvent_t *>(&self);
}

const SoyMilk::Codec::FrameBufferImpl &soymilk_cast(const SoyMilk_FrameBuffer_t *self) {
  return *reinterpret_cast<const SoyMilk::Codec::FrameBufferImpl *>(self);
}

SoyMilk::Codec::FrameBufferImpl &soymilk_cast(SoyMilk_FrameBuffer_t *self) {
  return *reinterpret_cast<SoyMilk::Codec::FrameBufferImpl *>(self);
}

SoyMilk_FrameBuffer_t *soymilk_cast(SoyMilk::Codec::FrameBufferImpl &self) {
  return reinterpret_cast<SoyMilk_FrameBuffer_t *>(&self);
}

} // namespace

MilkTea_extern(SoyMilk_FrameEvent_GetNtrk, (const SoyMilk_FrameEvent_t *self, uint16_t *ntrk), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(ntrk);
  *ntrk = soymilk_cast(self).ntrk();
})

MilkTea_extern(SoyMilk_FrameEvent_GetLen, (const SoyMilk_FrameEvent_t *self, size_t *len), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(len);
  *len = soymilk_cast(self).msg().size();
})

MilkTea_extern(SoyMilk_FrameEvent_GetMsg, (const SoyMilk_FrameEvent_t *self, size_t idx, const MilkPowder_Message_t **msg), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(msg);
  size_t len = soymilk_cast(self).msg().size();
  if (idx >= len) {
    MilkTea_throwf(LogicError, "out of bound -- idx: %zu, len: %zu", idx, len);
  }
  *msg = soymilk_cast(self).msg()[idx].get();
})

MilkTea_extern(SoyMilk_FrameBuffer_Clone, (SoyMilk_FrameBuffer_t **self, const SoyMilk_FrameBuffer_t *another), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(another);
  *self = soymilk_cast(*new SoyMilk::Codec::FrameBufferImpl(soymilk_cast(another)));
})

MilkTea_extern(SoyMilk_FrameBuffer_Destroy, (SoyMilk_FrameBuffer_t *self), {
  MilkTea_nonnull(self);
  delete &soymilk_cast(self);
})

MilkTea_extern(SoyMilk_FrameBuffer_GetTime, (const SoyMilk_FrameBuffer_t *self, int64_t *time), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(time);
  *time = soymilk_cast(self).time().count();
})

MilkTea_extern(SoyMilk_FrameBuffer_GetLen, (const SoyMilk_FrameBuffer_t *self, size_t *len), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(len);
  *len = soymilk_cast(self).items().size();
})

MilkTea_extern(SoyMilk_FrameBuffer_GetItem, (const SoyMilk_FrameBuffer_t *self, size_t idx, const SoyMilk_FrameEvent_t **item), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  size_t len = soymilk_cast(self).items().size();
  if (idx >= len) {
    MilkTea_throwf(LogicError, "out of bound -- idx: %zu, len: %zu", idx, len);
  }
  *item = soymilk_cast(soymilk_cast(self).items()[idx]);
})
