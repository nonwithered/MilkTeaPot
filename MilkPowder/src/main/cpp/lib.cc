#include <milkpowder/common.h>

#include "midi.h"
#include "track.h"
#include "message.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace {

constexpr char TAG[] = "MilkPowder";

template<typename T>
struct FromRawType;

template<>
struct FromRawType<MilkPowder_Midi_t> {
  using target = MilkPowder::MidiImpl;
};

template<>
struct FromRawType<MilkPowder_Track_t> {
  using target = MilkPowder::TrackImpl;
};

template<>
struct FromRawType<MilkPowder_Message_t> {
  using target = MilkPowder::MessageImpl;
};

template<>
struct FromRawType<MilkPowder_Event_t> {
  using target = MilkPowder::EventImpl;
};

template<>
struct FromRawType<MilkPowder_Meta_t> {
  using target = MilkPowder::MetaImpl;
};

template<>
struct FromRawType<MilkPowder_Sysex_t> {
  using target = MilkPowder::SysexImpl;
};

template<typename T>
typename FromRawType<T>::target &milkpowder_cast(T *it) {
  return *reinterpret_cast<typename FromRawType<T>::target *>(it);
}

template<typename T>
const typename FromRawType<T>::target &milkpowder_cast(const T *it) {
  return *reinterpret_cast<const typename FromRawType<T>::target *>(it);
}

template<typename T>
typename T::mapping::raw_type *milkpowder_cast(T &it) {
  return reinterpret_cast<typename T::mapping::raw_type *>(&it);
}

template<typename T>
const typename T::mapping::raw_type *milkpowder_cast(const T &it) {
  return reinterpret_cast<const typename T::mapping::raw_type *>(&it);
}

std::function<std::tuple<uint8_t, bool>()> ByteReader_cast(MilkPowder_ByteReader_t reader) {
  return [reader]() -> auto {
    uint8_t bytes[1] = {0};
    size_t len = 1;
    MilkTea_invoke_panic(reader.invoke_, reader.self_, bytes, &len);
    return std::make_tuple(bytes[0], len != 0);
  };
}

template<typename T>
void MilkPowder_Parse(T **self, MilkPowder_ByteReader_t reader) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(reader.invoke_);
  *self = milkpowder_cast(FromRawType<T>::target::Parse(ByteReader_cast(reader)));
}

template<typename T>
void MilkPowder_Dump(const T *self, MilkPowder_ByteWriter_t writer) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(writer.invoke_);
  auto vec = milkpowder_cast(self).Dump();
  MilkTea_invoke_panic(writer.invoke_, writer.self_, vec.data(), static_cast<size_t>(vec.size()));
}

template<typename T>
void MilkPowder_Destroy(T *self) {
  MilkTea_nonnull(self);
  delete &milkpowder_cast(self);
}

template<typename T>
void MilkPowder_Clone(const T *self, T **another) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(another);
  *another = milkpowder_cast(*new typename FromRawType<T>::target(milkpowder_cast(self)));
}

template<typename T>
void MilkPowder_Message_From(MilkPowder_Message_t **self, T *item) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  *self = milkpowder_cast(dynamic_cast<MilkPowder::MessageImpl &>(milkpowder_cast(item)));
}

template<typename T>
void MilkPowder_Message_Is(const MilkPowder_Message_t *self, bool *item) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  *item = T::message_is(milkpowder_cast(self));
}

template<typename T>
void MilkPowder_Message_To(const MilkPowder_Message_t *self, const T **item) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  const auto *p = dynamic_cast<const typename FromRawType<T>::target *>(&milkpowder_cast(self));
  if (p == nullptr) {
    MilkTea_throw(LogicError, "must ensure the type is matched");
  }
  *item = milkpowder_cast(*p);
}

template<typename T>
void MilkPowder_From_Message(T **self, MilkPowder_Message_t *item) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  auto *p = dynamic_cast<typename FromRawType<T>::target *>(&milkpowder_cast(item));
  if (p == nullptr) {
    MilkTea_throw(LogicError, "must ensure the type is matched");
  }
  *self = milkpowder_cast(*p);
}

template<typename T>
void MilkPowder_To_Message(const T *self, const MilkPowder_Message_t **item) {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  *item = milkpowder_cast(dynamic_cast<const MilkPowder::MessageImpl &>(milkpowder_cast(self)));
}

MilkPowder_Sysex_Item_t Make_Sysex_Item(uint32_t delta, uint32_t length, const uint8_t *args) {
  return {
    .delta_ = delta,
    .length_ = length,
    .args_ = args,
  };
}

MilkPowder_Sysex_Item_mut_t Make_Sysex_Item_mut(uint32_t &delta, uint32_t length, uint8_t *args) {
  return {
    .delta_ = &delta,
    .length_ = length,
    .args_ = args,
  };
}

} // namespace

// Midi

MilkTea_extern(MilkPowder_Midi_Parse, (MilkPowder_Midi_t **self, MilkPowder_ByteReader_t reader), {
  MilkPowder_Parse(self, reader);
})

MilkTea_extern(MilkPowder_Midi_Create, (MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]), {
  MilkTea_nonnull(self);
  if (ntrks != 0) {
    MilkTea_nonnull(items);
  } else {
    MilkTea_logW("MilkPowder_Midi_Create: ntrks equals 0");
  }
  std::vector<std::unique_ptr<MilkPowder::TrackImpl>> vec;
  for (uint16_t i = 0; i < ntrks; ++i) {
    vec.emplace_back(&milkpowder_cast(items[i]));
  }
  *self = milkpowder_cast(*new MilkPowder::MidiImpl(format, division, std::move(vec)));
})

MilkTea_extern(MilkPowder_Midi_Clone, (const MilkPowder_Midi_t *self, MilkPowder_Midi_t **another), {
  MilkPowder_Clone(self, another);
})

MilkTea_extern(MilkPowder_Midi_Destroy, (MilkPowder_Midi_t *self), {
  MilkPowder_Destroy(self);
})

MilkTea_extern(MilkPowder_Midi_GetFormat, (const MilkPowder_Midi_t *self, uint16_t *format), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(format);
  *format = milkpowder_cast(self).format();
})

MilkTea_extern(MilkPowder_Midi_SetFormat, (MilkPowder_Midi_t *self, uint16_t format), {
  MilkTea_nonnull(self);
  if (format > 0x0002) {
    MilkTea_logW("set format %" PRIu16, format);
  }
  milkpowder_cast(self).format(format);
})

MilkTea_extern(MilkPowder_Midi_GetNtrks, (const MilkPowder_Midi_t *self, uint16_t *ntrks), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(ntrks);
  *ntrks = static_cast<uint16_t>(milkpowder_cast(self).items().size());
})

MilkTea_extern(MilkPowder_Midi_GetDivision, (const MilkPowder_Midi_t *self, uint16_t *division), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(division);
  *division = milkpowder_cast(self).division();
})

MilkTea_extern(MilkPowder_Midi_SetDivision, (MilkPowder_Midi_t *self, uint16_t division), {
  MilkTea_nonnull(self);
  milkpowder_cast(self).division(division);
})

MilkTea_extern(MilkPowder_Midi_GetTrack, (const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  auto size = static_cast<uint16_t>(milkpowder_cast(self).items().size());
  if (index >= size) {
    MilkTea_throwf(InvalidParam, "index: %" PRIu16 ", size: %" PRIu16, index, size);
  }
  *item = milkpowder_cast(*milkpowder_cast(self).items()[index].get());
})

MilkTea_extern(MilkPowder_Midi_AllTrack, (MilkPowder_Midi_t *self, MilkPowder_Track_Consumer_t consumer), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(consumer.invoke_);
  auto &items = milkpowder_cast(self).items();
  std::for_each(items.begin(), items.end(), [consumer](const std::unique_ptr<MilkPowder::TrackImpl> &it) {
    MilkTea_Function_Invoke(consumer, milkpowder_cast(*it.get()));
  });
})

MilkTea_extern(MilkPowder_Midi_Dump, (const MilkPowder_Midi_t *self, MilkPowder_ByteWriter_t writer), {
  MilkPowder_Dump(self, writer);
})

// Track

MilkTea_extern(MilkPowder_Track_Parse, (MilkPowder_Track_t **self, MilkPowder_ByteReader_t reader), {
  MilkPowder_Parse(self, reader);
})

MilkTea_extern(MilkPowder_Track_Create, (MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length), {
  MilkTea_nonnull(self);
  if (length != 0) {
    MilkTea_nonnull(items);
  } else {
    MilkTea_logW("MilkPowder_Track_Create: length equals 0");
  }
  std::vector<std::unique_ptr<MilkPowder::MessageImpl>> vec;
  for (uint16_t i = 0; i < length; ++i) {
    vec.emplace_back(&milkpowder_cast(items[i]));
  }
  *self = milkpowder_cast(*new MilkPowder::TrackImpl(std::move(vec)));
})

MilkTea_extern(MilkPowder_Track_Clone, (const MilkPowder_Track_t *self, MilkPowder_Track_t **another), {
  MilkPowder_Clone(self, another);
})

MilkTea_extern(MilkPowder_Track_Destroy, (MilkPowder_Track_t *self), {
  MilkPowder_Destroy(self);
})

MilkTea_extern(MilkPowder_Track_GetCount, (const MilkPowder_Track_t *self, uint32_t *count), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(count);
  *count = static_cast<uint32_t>(milkpowder_cast(self).items().size());
})

MilkTea_extern(MilkPowder_Track_GetMessage, (const MilkPowder_Track_t *self, uint32_t index, const MilkPowder_Message_t **item), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  auto size = static_cast<uint32_t>(milkpowder_cast(self).items().size());
  if (index >= size) {
    MilkTea_throwf(InvalidParam, "index: %" PRIu32 ", size: %" PRIu32, index, size);
  }
  *item = milkpowder_cast(*milkpowder_cast(self).items()[index].get());
})

MilkTea_extern(MilkPowder_Track_AllMessage, (MilkPowder_Track_t *self, MilkPowder_Message_Consumer_t consumer), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(consumer.invoke_);
  auto &items = milkpowder_cast(self).items();
  std::for_each(items.begin(), items.end(), [consumer](const std::unique_ptr<MilkPowder::MessageImpl> &it) {
    MilkTea_Function_Invoke(consumer, milkpowder_cast(*it.get()));
  });
})

MilkTea_extern(MilkPowder_Track_Dump, (const MilkPowder_Track_t *self, MilkPowder_ByteWriter_t writer), {
  MilkPowder_Dump(self, writer);
})

// Message

MilkTea_extern(MilkPowder_Message_Parse, (MilkPowder_Message_t **self, MilkPowder_ByteReader_t reader, uint8_t last), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(reader.invoke_);
  *self = milkpowder_cast(MilkPowder::MessageImpl::Parse(ByteReader_cast(reader), last));
})

MilkTea_extern(MilkPowder_Message_Clone, (const MilkPowder_Message_t *self, MilkPowder_Message_t **another), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(another);
  *another = milkpowder_cast(milkpowder_cast(self).Clone());
})

MilkTea_extern(MilkPowder_Message_Destroy, (MilkPowder_Message_t *self), {
  MilkPowder_Destroy(self);
})

MilkTea_extern(MilkPowder_Message_SetDelta, (MilkPowder_Message_t *self, uint32_t delta), {
  MilkTea_nonnull(self);
  milkpowder_cast(self).delta(delta);
})

MilkTea_extern(MilkPowder_Message_GetDelta, (const MilkPowder_Message_t *self, uint32_t *delta), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(delta);
  *delta = milkpowder_cast(self).delta();
})

MilkTea_extern(MilkPowder_Message_GetType, (const MilkPowder_Message_t *self, uint8_t *type), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(type);
  *type = milkpowder_cast(self).type();
})

MilkTea_extern(MilkPowder_Message_Dump, (const MilkPowder_Message_t *self, MilkPowder_ByteWriter_t writer), {
  MilkPowder_Dump(self, writer);
})

MilkTea_extern(MilkPowder_Message_FromEvent, (MilkPowder_Message_t **self, MilkPowder_Event_t *item), {
  MilkPowder_Message_From(self, item);
})

MilkTea_extern(MilkPowder_Message_FromMeta, (MilkPowder_Message_t **self, MilkPowder_Meta_t *item), {
  MilkPowder_Message_From(self, item);
})

MilkTea_extern(MilkPowder_Message_FromSysex, (MilkPowder_Message_t **self, MilkPowder_Sysex_t *item), {
  MilkPowder_Message_From(self, item);
})

MilkTea_extern(MilkPowder_Message_IsEvent, (const MilkPowder_Message_t *self, bool *item), {
  MilkPowder_Message_Is<MilkPowder::EventImpl>(self, item);
})

MilkTea_extern(MilkPowder_Message_IsMeta, (const MilkPowder_Message_t *self, bool *item), {
  MilkPowder_Message_Is<MilkPowder::MetaImpl>(self, item);
})

MilkTea_extern(MilkPowder_Message_IsSysex, (const MilkPowder_Message_t *self, bool *item), {
  MilkPowder_Message_Is<MilkPowder::SysexImpl>(self, item);
})

MilkTea_extern(MilkPowder_Message_ToEvent, (const MilkPowder_Message_t *self, const MilkPowder_Event_t **item), {
  MilkPowder_Message_To(self, item);
})

MilkTea_extern(MilkPowder_Message_ToMeta, (const MilkPowder_Message_t *self, const MilkPowder_Meta_t **item), {
  MilkPowder_Message_To(self, item);
})

MilkTea_extern(MilkPowder_Message_ToSysex, (const MilkPowder_Message_t *self, const MilkPowder_Sysex_t **item), {
  MilkPowder_Message_To(self, item);
})

// Event

MilkTea_extern(MilkPowder_Event_Parse, (MilkPowder_Event_t **self, MilkPowder_ByteReader_t reader, uint8_t last), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(reader.invoke_);
  *self = milkpowder_cast(MilkPowder::EventImpl::Parse(ByteReader_cast(reader), last));
})

MilkTea_extern(MilkPowder_Event_Create, (MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1), {
  MilkTea_nonnull(self);
  *self = milkpowder_cast(*new MilkPowder::EventImpl(delta, type, std::make_tuple(arg0, arg1)));
})

MilkTea_extern(MilkPowder_Event_Clone, (const MilkPowder_Event_t *self, MilkPowder_Event_t **another), {
  MilkPowder_Clone(self, another);
})

MilkTea_extern(MilkPowder_Event_Destroy, (MilkPowder_Event_t *self), {
  MilkPowder_Destroy(self);
})

MilkTea_extern(MilkPowder_Event_GetType, (const MilkPowder_Event_t *self, uint8_t *type), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(type);
  *type = milkpowder_cast(self).type();
})

MilkTea_extern(MilkPowder_Event_GetArgs, (const MilkPowder_Event_t *self, uint8_t *args), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(args);
  uint8_t type = milkpowder_cast(self).type() & 0xf0;
  if (type != 0xc0 && type != 0xd0) {
    std::tie(args[0], args[1]) = milkpowder_cast(self).args();
  } else {
    args[0] = std::get<0>(milkpowder_cast(self).args());
  }
})

MilkTea_extern(MilkPowder_Event_Dump, (const MilkPowder_Event_t *self, MilkPowder_ByteWriter_t writer), {
  MilkPowder_Dump(self, writer);
})

MilkTea_extern(MilkPowder_Event_FromMessage, (MilkPowder_Event_t **self, MilkPowder_Message_t *item), {
  MilkPowder_From_Message(self, item);
})

MilkTea_extern(MilkPowder_Event_ToMessage, (const MilkPowder_Event_t *self, const MilkPowder_Message_t **item), {
  MilkPowder_To_Message(self, item);
})

// Meta

MilkTea_extern(MilkPowder_Meta_Parse, (MilkPowder_Meta_t **self, MilkPowder_ByteReader_t reader), {
  MilkPowder_Parse(self, reader);
})

MilkTea_extern(MilkPowder_Meta_Create, (MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t args[], uint32_t length), {
  MilkTea_nonnull(self);
  if (length != 0) {
    MilkTea_nonnull(args);
  } else {
    MilkTea_logW("MilkPowder_Meta_Create: length equals 0");
  }
  *self = milkpowder_cast(*new MilkPowder::MetaImpl(delta, type, std::vector<uint8_t>(args, args + length)));
})

MilkTea_extern(MilkPowder_Meta_Clone, (const MilkPowder_Meta_t *self, MilkPowder_Meta_t **another), {
  MilkPowder_Clone(self, another);
})

MilkTea_extern(MilkPowder_Meta_Destroy, (MilkPowder_Meta_t *self), {
  MilkPowder_Destroy(self);
})

MilkTea_extern(MilkPowder_Meta_GetType, (const MilkPowder_Meta_t *self, uint8_t *type), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(type);
  *type = milkpowder_cast(self).type();
})

MilkTea_extern(MilkPowder_Meta_GetArgs, (const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(args);
  MilkTea_nonnull(length);
  *length = static_cast<uint32_t>(milkpowder_cast(self).args().size());
  if (*length != 0) {
    *args = milkpowder_cast(self).args().data();
  } else {
    *args = nullptr;
  }
})

MilkTea_extern(MilkPowder_Meta_Dump, (const MilkPowder_Meta_t *self, MilkPowder_ByteWriter_t writer), {
  MilkPowder_Dump(self, writer);
})

MilkTea_extern(MilkPowder_Meta_FromMessage, (MilkPowder_Meta_t **self, MilkPowder_Message_t *item), {
  MilkPowder_From_Message(self, item);
})

MilkTea_extern(MilkPowder_Meta_ToMessage, (const MilkPowder_Meta_t *self, const MilkPowder_Message_t **item), {
  MilkPowder_To_Message(self, item);
})

// Sysex

MilkTea_extern(MilkPowder_Sysex_Parse, (MilkPowder_Sysex_t **self, MilkPowder_ByteReader_t reader), {
  MilkPowder_Parse(self, reader);
})

MilkTea_extern(MilkPowder_Sysex_Create, (MilkPowder_Sysex_t **self, uint32_t size, const MilkPowder_Sysex_Item_t items[]), {
  MilkTea_nonnull(self);
  if (size != 0) {
    MilkTea_nonnull(items);
  } else {
    MilkTea_logW("MilkPowder_Sysex_Create: size equals 0");
  }
  std::vector<MilkPowder::SysexImpl::Item> vec;
  std::for_each(items, items + size, [&vec](const MilkPowder_Sysex_Item_t &it) {
    vec.emplace_back(it.delta_, std::vector<uint8_t>(it.args_, it.args_ + it.length_));
  });
  *self = milkpowder_cast(*new MilkPowder::SysexImpl(std::move(vec)));
})

MilkTea_extern(MilkPowder_Sysex_Clone, (const MilkPowder_Sysex_t *self, MilkPowder_Sysex_t **another), {
  MilkPowder_Clone(self, another);
})

MilkTea_extern(MilkPowder_Sysex_Destroy, (MilkPowder_Sysex_t *self), {
  MilkPowder_Destroy(self);
})

MilkTea_extern(MilkPowder_Sysex_GetCount, (const MilkPowder_Sysex_t *self, uint32_t *count), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(count);
  *count = static_cast<uint32_t>(milkpowder_cast(self).items().size());
})

MilkTea_extern(MilkPowder_Sysex_GetItem, (const MilkPowder_Sysex_t *self, uint32_t index, MilkPowder_Sysex_Item_t *item), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(item);
  auto size = static_cast<uint32_t>(milkpowder_cast(self).items().size());
  if (index >= size) {
    MilkTea_throwf(InvalidParam, "index: %" PRIu32 ", size: %" PRIu32, index, size);
  }
  const auto &it = milkpowder_cast(self).items()[index];
  *item = Make_Sysex_Item(
    it.delta_,
    static_cast<uint32_t>(it.args_.size()),
    it.args_.data()
  );
})

MilkTea_extern(MilkPowder_Sysex_AllItem, (MilkPowder_Sysex_t *self, MilkPowder_Sysex_Item_Consumer_t consumer), {
  MilkTea_nonnull(self);
  MilkTea_nonnull(consumer.invoke_);
  auto &items = milkpowder_cast(self).items();
  std::for_each(items.begin(), items.end(), [consumer](MilkPowder::SysexImpl::Item &it) {
    MilkTea_Function_Invoke(consumer, Make_Sysex_Item_mut(it.delta_, static_cast<uint32_t>(it.args_.size()), it.args_.data()));
  });
})

MilkTea_extern(MilkPowder_Sysex_Dump, (const MilkPowder_Sysex_t *self, MilkPowder_ByteWriter_t writer), {
  MilkPowder_Dump(self, writer);
})

MilkTea_extern(MilkPowder_Sysex_FromMessage, (MilkPowder_Sysex_t **self, MilkPowder_Message_t *item), {
  MilkPowder_From_Message(self, item);
})

MilkTea_extern(MilkPowder_Sysex_ToMessage, (const MilkPowder_Sysex_t *self, const MilkPowder_Message_t **item), {
  MilkPowder_To_Message(self, item);
})
