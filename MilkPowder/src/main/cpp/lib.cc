#include <milkpowder.h>

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

constexpr char TAG[] = "api";

const char *ExceptWhat(const char *what = nullptr) {
  thread_local static std::string what_;
  if (what != nullptr) {
    what_ = what;
  }
  return what_.data();
}

inline MilkPowder_Err_t milkpowder_errno_map(MilkPowder::Except::Type type) {
  switch (type) {
    case MilkPowder::Except::Type::Assertion: return MilkPowder_Err_t::Assertion;
    case MilkPowder::Except::Type::NullPointer: return MilkPowder_Err_t::NullPointer;
    case MilkPowder::Except::Type::Unsupported: return MilkPowder_Err_t::Unsupported;
    case MilkPowder::Except::Type::EndOfFile: return MilkPowder_Err_t::EndOfFile;
    case MilkPowder::Except::Type::InvalidParam: return MilkPowder_Err_t::InvalidParam;
    case MilkPowder::Except::Type::LogicError: return MilkPowder_Err_t::LogicError;
    default: return MilkPowder_Err_t::Unknown;
  }
}

#define API_IMPL(section, list, block) \
MilkPowder_API MilkPowder_Err_t \
section list { \
  LOG_PRINT(DEBUG, "api_begin", #section); \
  MilkPowder::Defer defer([]() -> void { LOG_PRINT(DEBUG, "api_end", #section); }); \
  try { \
    block \
  } catch (MilkPowder::Except &e) { \
    ExceptWhat(e.what()); \
    return milkpowder_errno_map(e.type()); \
  } catch (std::exception &e) { \
    ExceptWhat(e.what()); \
    return MilkPowder_Err_t::Unknown; \
  } \
  ExceptWhat(""); \
  return MilkPowder_Err_t::Nil; \
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
  THROW_IF_NULL(self);
  THROW_IF_NULL(callback);
  *self = milkpowder_cast(milkpowder_cast_map<T>::Type::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }).release());
}

template<typename T>
void MilkPowder_Dump(const T *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)) {
  THROW_IF_NULL(self);
  THROW_IF_NULL(callback);
  std::vector<uint8_t> vec;
  milkpowder_cast(self)->Dump(vec);
  callback(obj, vec.data(), static_cast<size_t>(vec.size()));
}

template<typename T>
void MilkPowder_Destroy(T *self) {
  THROW_IF_NULL(self);
  delete milkpowder_cast(self);
}

template<typename T>
void MilkPowder_Message_From(MilkPowder_Message_t **self, T *item) {
  THROW_IF_NULL(self);
  THROW_IF_NULL(item);
  *self = milkpowder_cast(dynamic_cast<MilkPowder::Message *>(milkpowder_cast(item)));
}

template<bool (MilkPowder::Message::* Is)() const>
void MilkPowder_Message_Is(const MilkPowder_Message_t *self, bool *item) {
  THROW_IF_NULL(self);
  THROW_IF_NULL(item);
  *item = (milkpowder_cast(self)->*Is)();
}

template<bool (MilkPowder::Message::* Is)() const, typename T>
void MilkPowder_Message_To(const MilkPowder_Message_t *self, T **item) {
  THROW_IF_NULL(self);
  THROW_IF_NULL(item);
  if (!(milkpowder_cast(self)->*Is)()) {
    THROW(LogicError, "must ensure the type is matched");
  }
  *item = milkpowder_cast(dynamic_cast<typename milkpowder_cast_map<T>::Type *>(milkpowder_cast(self)));
}

template<typename T>
void MilkPowder_Clone(const T *self, T **another) {
  THROW_IF_NULL(self);
  THROW_IF_NULL(another);
  *another = milkpowder_cast(new typename milkpowder_cast_map<T>::Type(*milkpowder_cast(self)));
}

void LogDefault(const char *, const char *) {
}

std::function<void(const char *, const char *)> LogCallback(void *obj, void (*callback)(void *obj, const char *tag, const char *msg)) {
  return [obj, callback](const char *tag, const char *msg) -> void {
    callback(obj, tag, msg);
  };
}

inline MilkPowder::Log::Level milkpowder_log_map(MilkPowder_LogLevel_t level) {
  switch (level) {
      case MilkPowder_LogLevel_t::DEBUG: return MilkPowder::Log::Level::DEBUG;
      case MilkPowder_LogLevel_t::INFO: return MilkPowder::Log::Level::INFO;
      case MilkPowder_LogLevel_t::WARN: return MilkPowder::Log::Level::WARN;
      case MilkPowder_LogLevel_t::ERROR: return MilkPowder::Log::Level::ERROR;
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
MilkPowder_Log_Init(MilkPowder_Logger_t config) {
  MilkPowder::Log logger(
    LogCallback(config.obj, config.debug),
    LogCallback(config.obj, config.info),
    LogCallback(config.obj, config.warn),
    LogCallback(config.obj, config.error),
    milkpowder_log_map(config.level));
  MilkPowder::Log::Instance(&logger);
}

MilkPowder_API const char *
MilkPowder_Err_What() {
  return ExceptWhat();
}

// Midi

API_IMPL(MilkPowder_Midi_Parse, (MilkPowder_Midi_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte)), {
  MilkPowder_Parse(self, obj, callback);
})

API_IMPL(MilkPowder_Midi_Create, (MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]), {
  THROW_IF_NULL(self);
  if (ntrks != 0) {
    THROW_IF_NULL(items);
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
  THROW_IF_NULL(self);
  THROW_IF_NULL(format);
  *format = milkpowder_cast(self)->format();
})

API_IMPL(MilkPowder_Midi_GetNtrks, (const MilkPowder_Midi_t *self, uint16_t *ntrks), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(ntrks);
  *ntrks = static_cast<uint16_t>(milkpowder_cast(self)->items().size());
})

API_IMPL(MilkPowder_Midi_GetDivision, (const MilkPowder_Midi_t *self, uint16_t *division), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(division);
  *division = milkpowder_cast(self)->division();
})

API_IMPL(MilkPowder_Midi_GetTrack, (const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(item);
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
  THROW_IF_NULL(self);
  if (length != 0) {
    THROW_IF_NULL(items);
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
  THROW_IF_NULL(self);
  THROW_IF_NULL(callback);
  for (const auto &it : milkpowder_cast(self)->items()) {
    callback(obj, milkpowder_cast(it.get()));
  }
})

API_IMPL(MilkPowder_Track_Dump, (const MilkPowder_Track_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

// Message

API_IMPL(MilkPowder_Message_Parse, (MilkPowder_Message_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(callback);
  *self = milkpowder_cast(MilkPowder::Message::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }, last).release());
})

API_IMPL(MilkPowder_Message_Clone, (const MilkPowder_Message_t *self, MilkPowder_Message_t **another), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(another);
  *another = milkpowder_cast(milkpowder_cast(self)->Clone().release());
})

API_IMPL(MilkPowder_Message_Destroy, (MilkPowder_Message_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Message_SetDelta, (MilkPowder_Message_t *self, uint32_t delta), {
  THROW_IF_NULL(self);
  milkpowder_cast(self)->delta(delta);
})

API_IMPL(MilkPowder_Message_GetDelta, (const MilkPowder_Message_t *self, uint32_t *delta), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(delta);
  *delta = milkpowder_cast(self)->delta();
})

API_IMPL(MilkPowder_Message_GetType, (const MilkPowder_Message_t *self, uint8_t *type), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(type);
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
  THROW_IF_NULL(self);
  THROW_IF_NULL(callback);
  *self = milkpowder_cast(MilkPowder::Event::Parse([obj, callback]() -> std::tuple<uint8_t, bool> {
    uint8_t byte;
    bool ret = callback(obj, &byte);
    return std::make_tuple(byte, ret);
  }, last).release());
})

API_IMPL(MilkPowder_Event_Create, (MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1), {
  THROW_IF_NULL(self);
  *self = milkpowder_cast(new MilkPowder::Event(delta, type, std::make_tuple(arg0, arg1)));
})

API_IMPL(MilkPowder_Event_Clone, (const MilkPowder_Event_t *self, MilkPowder_Event_t **another), {
  MilkPowder_Clone(self, another);
})

API_IMPL(MilkPowder_Event_Destroy, (MilkPowder_Event_t *self), {
  MilkPowder_Destroy(self);
})

API_IMPL(MilkPowder_Event_GetType, (const MilkPowder_Event_t *self, uint8_t *type), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(type);
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Event_GetArgs, (const MilkPowder_Event_t *self, uint8_t *args), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(args);
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
  THROW_IF_NULL(self);
  if (length != 0) {
    THROW_IF_NULL(args);
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
  THROW_IF_NULL(self);
  THROW_IF_NULL(type);
  *type = milkpowder_cast(self)->type();
})

API_IMPL(MilkPowder_Meta_GetArgs, (const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length), {
  THROW_IF_NULL(self);
  THROW_IF_NULL(args);
  THROW_IF_NULL(length);
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
  THROW_IF_NULL(self);
  if (size != 0) {
    THROW_IF_NULL(delta);
    THROW_IF_NULL(args);
    THROW_IF_NULL(length);
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
  THROW_IF_NULL(self);
  for (const auto &it : milkpowder_cast(self)->items()) {
    callback(obj, static_cast<uint32_t>(std::get<0>(it)), std::get<1>(it).data(), static_cast<uint32_t>(std::get<1>(it).size()));
  }
})

API_IMPL(MilkPowder_Sysex_Dump, (const MilkPowder_Sysex_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len)), {
  MilkPowder_Dump(self, obj, callback);
})

} // extern "C"
