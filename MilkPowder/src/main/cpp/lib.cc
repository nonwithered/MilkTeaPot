#include "milkpowder.h"

#include "except.h"
#include "log.h"
#include "defer.h"

#include "midi.h"
#include "track.h"
#include "message.h"
#include "event.h"
#include "meta.h"
#include "sysex.h"

namespace {

inline MilkPowder_Errno_t milkpowder_errno_map(MilkPowder::Except::Type type) {
  switch (type) {
    case MilkPowder::Except::Type::Assertion: return MilkPowder_Errno_t::Assertion;
    case MilkPowder::Except::Type::NullPointer: return MilkPowder_Errno_t::NullPointer;
    case MilkPowder::Except::Type::Unsupported: return MilkPowder_Errno_t::Unsupported;
    case MilkPowder::Except::Type::EndOfFile: return MilkPowder_Errno_t::EndOfFile;
    case MilkPowder::Except::Type::InvalidParam: return MilkPowder_Errno_t::InvalidParam;
    case MilkPowder::Except::Type::LogicError: return MilkPowder_Errno_t::LogicError;
    default: return MilkPowder_Errno_t::Unknown;
  }
}

#define API_IMPL(section, list, block) \
MilkPowder_API MilkPowder_Errno_t \
section list { \
  LOG_PRINT(INFO, "api_begin", #section); \
  MilkPowder::Defer defer([]() -> void { LOG_PRINT(INFO, "api_end", #section); }); \
  try \
    block \
  catch (MilkPowder::Except &e) { \
    return milkpowder_errno_map(e.type()); \
  } \
  return MilkPowder_Errno_t::Nil; \
}

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
};
milkpowder_cast_map_(Midi)
milkpowder_cast_map_(Track)
milkpowder_cast_map_(Message)
milkpowder_cast_map_(Event)
milkpowder_cast_map_(Meta)
milkpowder_cast_map_(Sysex)
#undef milkpowder_cast_map_

#define milkpowder_cast_map_(T) \
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
  if (self == nullptr || callback == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(milkpowder_cast_map<T>::Type::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }).release());
}

template<typename T>
void MilkPowder_Destroy(T *self) {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
}

template<typename T>
void MilkPowder_Message_From(MilkPowder_Message_t **self, T *item) {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(dynamic_cast<MilkPowder::Message *>(milkpowder_cast(item)));
}

template<bool (MilkPowder::Message::* Is)() const>
void MilkPowder_Message_Is(const MilkPowder_Message_t *self, bool *item) {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *item = (milkpowder_cast(self)->*Is)();
}

template<bool (MilkPowder::Message::* Is)() const, typename T>
void MilkPowder_Message_To(const MilkPowder_Message_t *self, T **item) {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  if (!(milkpowder_cast(self)->*Is)()) {
    throw MilkPowder::Except(MilkPowder::Except::Type::LogicError);
  }
  *item = milkpowder_cast(dynamic_cast<typename milkpowder_cast_map<T>::Type *>(milkpowder_cast(self)));
}

template<typename T>
void MilkPowder_Clone(const T *self, T **another) {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(new typename milkpowder_cast_map<T>::Type(*milkpowder_cast(self)));
}

void LogDefault(const char *, const char *) {
}

std::function<void(const char *, const char *)> LogCallback(void *obj, void (*callback)(void *obj, const char *tag, const char *msg)) {
  return [obj, callback](const char *tag, const char *msg) -> void {
    callback(obj, tag, msg);
  };
}

inline MilkPowder::Log::Level milkpowder_log_map(MilkPowder_Log_Level_t level) {
  switch (level) {
      case MilkPowder_Log_Level_t::DEBUG: return MilkPowder::Log::Level::DEBUG;
      case MilkPowder_Log_Level_t::INFO: return MilkPowder::Log::Level::INFO;
      case MilkPowder_Log_Level_t::WARN: return MilkPowder::Log::Level::WARN;
      case MilkPowder_Log_Level_t::ERROR: return MilkPowder::Log::Level::ERROR;
      default: return MilkPowder::Log::Level::ASSERT;
  }
}

} // namespace

MilkPowder::Log &MilkPowder::Log::Instance(MilkPowder::Log *logger) {
  static MilkPowder::Log sLogger_(LogDefault, LogDefault, LogDefault, LogDefault, MilkPowder::Log::Level::ASSERT);
  if (logger != nullptr) {
    sLogger_ = *logger;
  }
  return sLogger_;
}

extern "C" {

MilkPowder_API void
MilkPowder_Log_Init(MilkPowder_Log_Config_t config) {
  MilkPowder::Log logger(
    LogCallback(config.obj, config.debug),
    LogCallback(config.obj, config.info),
    LogCallback(config.obj, config.warn),
    LogCallback(config.obj, config.error),
    milkpowder_log_map(config.level));
  MilkPowder::Log::Instance(&logger);
}

// Midi

API_IMPL(MilkPowder_Midi_Parse, (MilkPowder_Midi_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Midi_Create, (MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]), {
  if (self == nullptr || items == nullptr && ntrks != 0) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
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
  if (self == nullptr || format == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *format = milkpowder_cast(self)->format();
})

API_IMPL(MilkPowder_Midi_GetNtrks, (const MilkPowder_Midi_t *self, uint16_t *ntrks), {
  if (self == nullptr || ntrks == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *ntrks = static_cast<uint16_t>(milkpowder_cast(self)->items().size());
})

API_IMPL(MilkPowder_Midi_GetDivision, (const MilkPowder_Midi_t *self, uint16_t *division), {
  if (self == nullptr || division == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *division = milkpowder_cast(self)->division();
})

API_IMPL(MilkPowder_Midi_GetTrack, (const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  if (index >= static_cast<uint16_t>(milkpowder_cast(self)->items().size())) {
    throw MilkPowder::Except(MilkPowder::Except::Type::InvalidParam);
  }
  *item = milkpowder_cast(milkpowder_cast(self)->items()[index].get());
})

// Track

API_IMPL(MilkPowder_Track_Parse, (MilkPowder_Track_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Track_Create, (MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length), {
  if (self == nullptr || (length != 0 && items == nullptr)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
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
  if (self == nullptr || callback == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  for (const auto &it : milkpowder_cast(self)->items()) {
    callback(obj, milkpowder_cast(it.get()));
  }
})

// Message

API_IMPL(MilkPowder_Message_Parse, (MilkPowder_Message_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last), {
  if (self == nullptr || callback == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(MilkPowder::Message::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }, last).release());
})

API_IMPL(MilkPowder_Message_Clone, (const MilkPowder_Message_t *self, MilkPowder_Message_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(milkpowder_cast(self)->Clone().release());
})

API_IMPL(MilkPowder_Message_Destroy, (MilkPowder_Message_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Message_GetDelta, (const MilkPowder_Message_t *self, uint32_t *delta), {
  if (self == nullptr || delta == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *delta = milkpowder_cast(self)->delta();
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

// Event

API_IMPL(MilkPowder_Event_Parse, (MilkPowder_Event_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last), {
  if (self == nullptr || callback == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(MilkPowder::Event::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }, last).release());
})

API_IMPL(MilkPowder_Event_Create, (MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(new MilkPowder::Event(delta, type, std::make_tuple(arg0, arg1)));
})

API_IMPL(MilkPowder_Event_Clone, (const MilkPowder_Event_t *self, MilkPowder_Event_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Event_Destroy, (MilkPowder_Event_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Event_GetType, (const MilkPowder_Event_t *self, uint8_t *type), {
  if (self == nullptr || type == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Event_GetArgs, (const MilkPowder_Event_t *self, uint8_t *args), {
  if (self == nullptr || args == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  uint8_t type = milkpowder_cast(self)->type() & 0xf0;
  if (type != 0xc0 && type != 0xd0) {
    std::tie(args[0], args[1]) = milkpowder_cast(self)->args();
  } else {
    args[0] = std::get<0>(milkpowder_cast(self)->args());
  }
})

// Meta

API_IMPL(MilkPowder_Meta_Parse, (MilkPowder_Meta_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Meta_Create, (MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t *args, uint32_t length), {
  if (self == nullptr || (length != 0 && args == nullptr)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
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
  if (self == nullptr || type == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Meta_GetArgs, (const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length), {
  if (self == nullptr || args == nullptr || length == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *length = static_cast<uint32_t>(milkpowder_cast(self)->args().size());
  if (*length != 0) {
    *args = milkpowder_cast(self)->args().data();
  } else {
    *args = nullptr;
  }
})

// Sysex

API_IMPL(MilkPowder_Sysex_Parse, (MilkPowder_Sysex_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})


using Vec = std::vector<std::tuple<uint32_t, std::vector<uint8_t>>>;

API_IMPL(MilkPowder_Sysex_Create, (MilkPowder_Sysex_t **self, uint32_t delta[], const uint8_t *args[], uint32_t length[], uint32_t size), {
  if (self == nullptr || (size != 0 && (delta == nullptr || args == nullptr || length == nullptr))) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
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
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  for (const auto &it : milkpowder_cast(self)->items()) {
    callback(obj, static_cast<uint32_t>(std::get<0>(it)), std::get<1>(it).data(), static_cast<uint32_t>(std::get<1>(it).size()));
  }
})

} // extern "C"
