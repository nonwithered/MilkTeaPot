#include <milkpowder.h>

#include "util.h"

MilkTea_Exception_What_IMPL(MilkPowder)

extern "C"
MilkPowder_API const char *
MilkPowder_Exception_What() {
  return MilkPowder::MilkTea_Exception_What();
}

MilkTea_Logger_Log_IMPL(MilkPowder)

extern "C"
MilkPowder_API void
MilkPowder_Log_Init(MilkPowder_Logger_t log) {
  MilkTea::Logger log_(log);
  MilkPowder::Log(&log_);
}

#define API_IMPL(section, list, block) \
MilkPowder_API MilkPowder_Exception_t \
section list { \
  LOG_PRINT(DEBUG, TAG, "begin " #section); \
  MilkTea::Defer defer([]() -> void { LOG_PRINT(DEBUG, TAG, "end " #section); }); \
  MILKTEA_EXCEPTION_WITH(MilkPowder, block); \
}

#include "midi.h"
#include "track.h"
#include "message.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace {

constexpr char TAG[] = "milkpowder_api";

template<typename T>
struct milkpowder_cast_map {
  using Type = T;
};

#define milkpowder_cast_map_(T) \
template<> \
struct milkpowder_cast_map<const MilkPowder_##T##_t> { \
  using Type = const MilkPowder::T; \
}; \
template<> \
struct milkpowder_cast_map<MilkPowder_##T##_t> { \
  using Type = MilkPowder::T; \
}; \
template<> \
struct milkpowder_cast_map<const MilkPowder::T> { \
  using Type = const MilkPowder_##T##_t; \
}; \
template<> \
struct milkpowder_cast_map<MilkPowder::T> { \
  using Type = MilkPowder_##T##_t; \
};
milkpowder_cast_map_(Midi)
milkpowder_cast_map_(Track)
milkpowder_cast_map_(Message)
milkpowder_cast_map_(Event)
milkpowder_cast_map_(Meta)
milkpowder_cast_map_(Sysex)
#undef milkpowder_cast_map_

template<typename From, typename To = typename milkpowder_cast_map<From>::Type>
constexpr To *milkpowder_cast(From *it) {
  return reinterpret_cast<To *>(it);
}

template<typename T>
void MilkPowder_Parse(T **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)) {
  THROW_NULL(self);
  THROW_NULL(callback);
  *self = milkpowder_cast(milkpowder_cast_map<T>::Type::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }).release());
}

template<typename T>
void MilkPowder_Dump(const T *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)) {
  THROW_NULL(self);
  THROW_NULL(callback);
  std::vector<uint8_t> vec;
  milkpowder_cast(self)->Dump(vec);
  callback(obj, vec.data(), static_cast<size_t>(vec.size()));
}

template<typename T>
void MilkPowder_Destroy(T *self) {
  THROW_NULL(self);
  delete milkpowder_cast(self);
}

template<typename T>
void MilkPowder_Message_From(MilkPowder_Message_t **self, T *item) {
  THROW_NULL(self);
  THROW_NULL(item);
  *self = milkpowder_cast(dynamic_cast<MilkPowder::Message *>(milkpowder_cast(item)));
}

template<bool (MilkPowder::Message::* Is)() const>
void MilkPowder_Message_Is(const MilkPowder_Message_t *self, bool *item) {
  THROW_NULL(self);
  THROW_NULL(item);
  *item = (milkpowder_cast(self)->*Is)();
}

template<bool (MilkPowder::Message::* Is)() const, typename T>
void MilkPowder_Message_To(const MilkPowder_Message_t *self, T **item) {
  THROW_NULL(self);
  THROW_NULL(item);
  if (!(milkpowder_cast(self)->*Is)()) {
    THROW(LogicError, "must ensure the type is matched");
  }
  *item = milkpowder_cast(dynamic_cast<typename milkpowder_cast_map<T>::Type *>(milkpowder_cast(self)));
}

template<typename T>
void MilkPowder_Clone(const T *self, T **another) {
  THROW_NULL(self);
  THROW_NULL(another);
  *another = milkpowder_cast(new typename milkpowder_cast_map<T>::Type(*milkpowder_cast(self)));
}

} // namespace

extern "C" {

// Midi

API_IMPL(MilkPowder_Midi_Parse, (MilkPowder_Midi_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Midi_Create, (MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]), {
  THROW_NULL(self);
  if (ntrks != 0) {
    THROW_NULL(items);
  } else {
    LOG_PRINT(WARN, TAG, "MilkPowder_Midi_Create: ntrks equals 0");
  }
  std::vector<std::unique_ptr<MilkPowder::Track>> vec;
  for (uint16_t i = 0; i < ntrks; ++i) {
    vec.emplace_back(milkpowder_cast(items[i]));
  }
  *self = milkpowder_cast(new MilkPowder::Midi(format, division, std::move(vec)));
})

API_IMPL(MilkPowder_Midi_Clone, (const MilkPowder_Midi_t *self, MilkPowder_Midi_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Midi_Destroy, (MilkPowder_Midi_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Midi_GetFormat, (const MilkPowder_Midi_t *self, uint16_t *format), {
  THROW_NULL(self);
  THROW_NULL(format);
  *format = milkpowder_cast(self)->format();
})

API_IMPL(MilkPowder_Midi_GetNtrks, (const MilkPowder_Midi_t *self, uint16_t *ntrks), {
  THROW_NULL(self);
  THROW_NULL(ntrks);
  *ntrks = static_cast<uint16_t>(milkpowder_cast(self)->items().size());
})

API_IMPL(MilkPowder_Midi_GetDivision, (const MilkPowder_Midi_t *self, uint16_t *division), {
  THROW_NULL(self);
  THROW_NULL(division);
  *division = milkpowder_cast(self)->division();
})

API_IMPL(MilkPowder_Midi_GetTrack, (const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item), {
  THROW_NULL(self);
  THROW_NULL(item);
  if (index >= static_cast<uint16_t>(milkpowder_cast(self)->items().size())) {
    THROW_FORMAT(InvalidParam, "index: %" PRIu16 ", size: %" PRIu16, index, static_cast<uint16_t>(milkpowder_cast(self)->items().size()));
  }
  *item = milkpowder_cast(milkpowder_cast(self)->items()[index].get());
})

API_IMPL(MilkPowder_Midi_Dump, (const MilkPowder_Midi_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

// Track

API_IMPL(MilkPowder_Track_Parse, (MilkPowder_Track_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Track_Create, (MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length), {
  THROW_NULL(self);
  if (length != 0) {
    THROW_NULL(items);
  } else {
    LOG_PRINT(WARN, TAG, "MilkPowder_Track_Create: length equals 0");
  }
  std::vector<std::unique_ptr<MilkPowder::Message>> vec;
  for (uint16_t i = 0; i < length; ++i) {
    vec.emplace_back(milkpowder_cast(items[i]));
  }
  *self = milkpowder_cast(new MilkPowder::Track(std::move(vec)));
})

API_IMPL(MilkPowder_Track_Clone, (const MilkPowder_Track_t *self, MilkPowder_Track_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Track_Destroy, (MilkPowder_Track_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Track_GetMessages, (const MilkPowder_Track_t *self, void *obj, void (*callback)(void *obj, const MilkPowder_Message_t *item)), {
  THROW_NULL(self);
  THROW_NULL(callback);
  for (const auto &it : milkpowder_cast(self)->items()) {
    callback(obj, milkpowder_cast(it.get()));
  }
})

API_IMPL(MilkPowder_Track_Dump, (const MilkPowder_Track_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

// Message

API_IMPL(MilkPowder_Message_Parse, (MilkPowder_Message_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last), {
  THROW_NULL(self);
  THROW_NULL(callback);
  *self = milkpowder_cast(MilkPowder::Message::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }, last).release());
})

API_IMPL(MilkPowder_Message_Clone, (const MilkPowder_Message_t *self, MilkPowder_Message_t **another), {
  THROW_NULL(self);
  THROW_NULL(another);
  *another = milkpowder_cast(milkpowder_cast(self)->Clone().release());
})

API_IMPL(MilkPowder_Message_Destroy, (MilkPowder_Message_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Message_SetDelta, (MilkPowder_Message_t *self, uint32_t delta), {
  THROW_NULL(self);
  milkpowder_cast(self)->delta(delta);
})

API_IMPL(MilkPowder_Message_GetDelta, (const MilkPowder_Message_t *self, uint32_t *delta), {
  THROW_NULL(self);
  THROW_NULL(delta);
  *delta = milkpowder_cast(self)->delta();
})

API_IMPL(MilkPowder_Message_GetType, (const MilkPowder_Message_t *self, uint8_t *type), {
  THROW_NULL(self);
  THROW_NULL(type);
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Message_FromEvent, (MilkPowder_Message_t **self, MilkPowder_Event_t *item), {
  MilkPowder_Message_From(self, item);
})

API_IMPL(MilkPowder_Message_FromMeta, (MilkPowder_Message_t **self, MilkPowder_Meta_t *item), {
  MilkPowder_Message_From(self, item);
})

API_IMPL(MilkPowder_Message_FromSysex, (MilkPowder_Message_t **self, MilkPowder_Sysex_t *item), {
  MilkPowder_Message_From(self, item);
})

API_IMPL(MilkPowder_Message_IsEvent, (const MilkPowder_Message_t *self, bool *item), {
  MilkPowder_Message_Is<&MilkPowder::Message::IsEvent>(self, item);
})

API_IMPL(MilkPowder_Message_IsMeta, (const MilkPowder_Message_t *self, bool *item), {
  MilkPowder_Message_Is<&MilkPowder::Message::IsMeta>(self, item);
})

API_IMPL(MilkPowder_Message_IsSysex, (const MilkPowder_Message_t *self, bool *item), {
  MilkPowder_Message_Is<&MilkPowder::Message::IsSysex>(self, item);
})

API_IMPL(MilkPowder_Message_ToEvent, (const MilkPowder_Message_t *self, const MilkPowder_Event_t **item), {
  MilkPowder_Message_To<&MilkPowder::Message::IsEvent>(self, item);
})

API_IMPL(MilkPowder_Message_ToMeta, (const MilkPowder_Message_t *self, const MilkPowder_Meta_t **item), {
  MilkPowder_Message_To<&MilkPowder::Message::IsMeta>(self, item);
})

API_IMPL(MilkPowder_Message_ToSysex, (const MilkPowder_Message_t *self, const MilkPowder_Sysex_t **item), {
  MilkPowder_Message_To<&MilkPowder::Message::IsSysex>(self, item);
})

API_IMPL(MilkPowder_Message_Dump, (const MilkPowder_Message_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

// Event

API_IMPL(MilkPowder_Event_Parse, (MilkPowder_Event_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last), {
  THROW_NULL(self);
  THROW_NULL(callback);
  *self = milkpowder_cast(MilkPowder::Event::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }, last).release());
})

API_IMPL(MilkPowder_Event_Create, (MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1), {
  THROW_NULL(self);
  *self = milkpowder_cast(new MilkPowder::Event(delta, type, std::make_tuple(arg0, arg1)));
})

API_IMPL(MilkPowder_Event_Clone, (const MilkPowder_Event_t *self, MilkPowder_Event_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Event_Destroy, (MilkPowder_Event_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Event_GetType, (const MilkPowder_Event_t *self, uint8_t *type), {
  THROW_NULL(self);
  THROW_NULL(type);
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Event_GetArgs, (const MilkPowder_Event_t *self, uint8_t *args), {
  THROW_NULL(self);
  THROW_NULL(args);
  uint8_t type = milkpowder_cast(self)->type() & 0xf0;
  if (type != 0xc0 && type != 0xd0) {
    std::tie(args[0], args[1]) = milkpowder_cast(self)->args();
  } else {
    args[0] = std::get<0>(milkpowder_cast(self)->args());
  }
})

API_IMPL(MilkPowder_Event_Dump, (const MilkPowder_Event_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

// Meta

API_IMPL(MilkPowder_Meta_Parse, (MilkPowder_Meta_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Meta_Create, (MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t *args, uint32_t length), {
  THROW_NULL(self);
  if (length != 0) {
    THROW_NULL(args);
  } else {
    LOG_PRINT(WARN, TAG, "MilkPowder_Meta_Create: length equals 0");
  }
  *self = milkpowder_cast(new MilkPowder::Meta(delta, type, std::vector<uint8_t>(args, args + length)));
})

API_IMPL(MilkPowder_Meta_Clone, (const MilkPowder_Meta_t *self, MilkPowder_Meta_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Meta_Destroy, (MilkPowder_Meta_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Meta_GetType, (const MilkPowder_Meta_t *self, uint8_t *type), {
  THROW_NULL(self);
  THROW_NULL(type);
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Meta_GetArgs, (const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length), {
  THROW_NULL(self);
  THROW_NULL(args);
  THROW_NULL(length);
  *length = static_cast<uint32_t>(milkpowder_cast(self)->args().size());
  if (*length != 0) {
    *args = milkpowder_cast(self)->args().data();
  } else {
    *args = nullptr;
  }
})

API_IMPL(MilkPowder_Meta_Dump, (const MilkPowder_Meta_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

// Sysex

API_IMPL(MilkPowder_Sysex_Parse, (MilkPowder_Sysex_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

using Vec = std::vector<std::tuple<uint32_t, std::vector<uint8_t>>>;

API_IMPL(MilkPowder_Sysex_Create, (MilkPowder_Sysex_t **self, uint32_t delta[], const uint8_t *args[], uint32_t length[], uint32_t size), {
  THROW_NULL(self);
  if (size != 0) {
    THROW_NULL(delta);
    THROW_NULL(args);
    THROW_NULL(length);
  } else {
    LOG_PRINT(WARN, TAG, "MilkPowder_Sysex_Create: size equals 0");
  }
  Vec vec;
  for (uint32_t i = 0; i < size; ++i) {
    vec.emplace_back(delta[i], std::vector<uint8_t>(args[i], args[i] + length[i]));
  }
  *self = milkpowder_cast(new MilkPowder::Sysex(std::move(vec)));
})

API_IMPL(MilkPowder_Sysex_Clone, (const MilkPowder_Sysex_t *self, MilkPowder_Sysex_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Sysex_Destroy, (MilkPowder_Sysex_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Sysex_GetArgs, (const MilkPowder_Sysex_t *self, void *obj, void (*callback)(void *obj, uint32_t delta, const uint8_t *args, uint32_t length)), {
  THROW_NULL(self);
  for (const auto &it : milkpowder_cast(self)->items()) {
    callback(obj, static_cast<uint32_t>(std::get<0>(it)), std::get<1>(it).data(), static_cast<uint32_t>(std::get<1>(it).size()));
  }
})

API_IMPL(MilkPowder_Sysex_Dump, (const MilkPowder_Sysex_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

} // extern "C"
