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

constexpr char TAG_API_BAGIN[] = "api_begin";
constexpr char TAG_API_END[] = "api_end";

inline void LogBegin(const char *section) {
  MilkPowder::Log::Print(MilkPowder::Log::Level::DEBUG, TAG_API_BAGIN, section);
}

inline void LogEnd(const char *section) {
  MilkPowder::Log::Print(MilkPowder::Log::Level::DEBUG, TAG_API_END, section);
}

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
  constexpr char TAG[] = #section; \
  LogBegin(TAG); \
  MilkPowder::Defer defer([TAG]() -> void { LogEnd(TAG); }); \
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
struct milkpowder_cast_map<const MilkPowder_##T##_t *> { \
  using Type = const MilkPowder::T *; \
}; \
template<> \
struct milkpowder_cast_map<MilkPowder_##T##_t *> { \
  using Type = MilkPowder::T *; \
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
struct milkpowder_cast_map<const MilkPowder::T *> { \
  using Type = const MilkPowder_##T##_t *; \
}; \
template<> \
struct milkpowder_cast_map<MilkPowder::T *> { \
  using Type = MilkPowder_##T##_t *; \
};
milkpowder_cast_map_(Midi)
milkpowder_cast_map_(Track)
milkpowder_cast_map_(Message)
milkpowder_cast_map_(Event)
milkpowder_cast_map_(Meta)
milkpowder_cast_map_(Sysex)
#undef milkpowder_cast_map_

template<typename From, typename To = typename milkpowder_cast_map<From>::Type>
constexpr To milkpowder_cast(From it) {
  return reinterpret_cast<To>(it);
}

void LogDefault(const char *, const char *) {
}

} // namespace

MilkPowder::Log MilkPowder::Log::Instance(MilkPowder::Log *logger) {
  static MilkPowder::Log sLogger_(LogDefault, LogDefault, LogDefault, LogDefault, MilkPowder::Log::Level::ASSERT);
  if (logger != nullptr) {
    sLogger_ = *logger;
  }
  return sLogger_;
}

extern "C" {

MilkPowder_API void
MilkPowder_Log_Init(MilkPowder_Log_Config_t config) {
  std::function<void(const char *, const char *)> debug = [config](const char *tag, const char *msg) -> void {
    config.debug(config.obj, tag, msg);
  };
  std::function<void(const char *, const char *)> info = [config](const char *tag, const char *msg) -> void {
    config.info(config.obj, tag, msg);
  };
  std::function<void(const char *, const char *)> warn = [config](const char *tag, const char *msg) -> void {
    config.warn(config.obj, tag, msg);
  };
  std::function<void(const char *, const char *)> error = [config](const char *tag, const char *msg) -> void {
    config.error(config.obj, tag, msg);
  };
  MilkPowder::Log::Level level = MilkPowder::Log::Level::ASSERT;
  switch (config.level) {
      case MilkPowder_Log_Level_t::DEBUG: level = MilkPowder::Log::Level::DEBUG; break;
      case MilkPowder_Log_Level_t::INFO: level = MilkPowder::Log::Level::INFO; break;
      case MilkPowder_Log_Level_t::WARN: level = MilkPowder::Log::Level::WARN; break;
      case MilkPowder_Log_Level_t::ERROR: level = MilkPowder::Log::Level::ERROR; break;
      default: break;
  }
  MilkPowder::Log logger(debug, info, warn, error, level);
  MilkPowder::Log::Instance(&logger);
}

// Midi

API_IMPL(MilkPowder_Midi_Parse, (MilkPowder_Midi_t **self, const uint8_t *bytes, uint32_t length, uint32_t *size), {
  if (self == nullptr || bytes == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  const uint8_t *cursor = bytes;
  auto ptr = MilkPowder::Midi::Parse(cursor, bytes + length);
  if (size != nullptr) {
    *size = cursor - bytes;
  }
  *self = milkpowder_cast(ptr.release());
})

API_IMPL(MilkPowder_Midi_Create, (MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]), {
  if (self == nullptr || items == nullptr && ntrks != 0) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  std::vector<std::unique_ptr<MilkPowder::Track>> vec(ntrks);
  for (uint16_t i = 0; i < ntrks; ++i) {
    vec.emplace_back(milkpowder_cast(items[i]));
  }
  *self = milkpowder_cast(new MilkPowder::Midi(format, division, std::move(vec)));
})

API_IMPL(MilkPowder_Midi_Clone, (const MilkPowder_Midi_t *self, MilkPowder_Midi_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(new MilkPowder::Midi(*milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Midi_Destroy, (MilkPowder_Midi_t *self), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
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

API_IMPL(MilkPowder_Track_Parse, (MilkPowder_Track_t **self, const uint8_t *bytes, uint32_t length, uint32_t *size), {
  if (self == nullptr || bytes == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  const uint8_t *cursor = bytes;
  auto ptr = MilkPowder::Track::Parse(cursor, bytes + length);
  if (size != nullptr) {
    *size = cursor - bytes;
  }
  *self = milkpowder_cast(ptr.release());
})

API_IMPL(MilkPowder_Track_Create, (MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length), {
  if (self == nullptr || (length != 0 && items == nullptr)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  std::vector<std::unique_ptr<MilkPowder::Message>> vec(length);
  for (uint16_t i = 0; i < length; ++i) {
    vec.emplace_back(milkpowder_cast(items[i]));
  }
  *self = milkpowder_cast(new MilkPowder::Track(std::move(vec)));
})

API_IMPL(MilkPowder_Track_Clone, (const MilkPowder_Track_t *self, MilkPowder_Track_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(new MilkPowder::Track(*milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Track_Destroy, (MilkPowder_Track_t *self), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
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

API_IMPL(MilkPowder_Message_Parse, (MilkPowder_Message_t **self, const uint8_t *bytes, uint32_t length, uint32_t *size, uint8_t last), {
  if (self == nullptr || bytes == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  const uint8_t *cursor = bytes;
  auto ptr = MilkPowder::Message::Parse(cursor, bytes + length, last);
  if (size != nullptr) {
    *size = cursor - bytes;
  }
  *self = milkpowder_cast(ptr.release());
})

API_IMPL(MilkPowder_Message_Clone, (const MilkPowder_Message_t *self, MilkPowder_Message_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(milkpowder_cast(self)->Clone().release());
})

API_IMPL(MilkPowder_Message_Destroy, (MilkPowder_Message_t *self), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
})

API_IMPL(MilkPowder_Message_GetDelta, (const MilkPowder_Message_t *self, uint32_t *delta), {
  if (self == nullptr || delta == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *delta = milkpowder_cast(self)->delta();
})

API_IMPL(MilkPowder_Message_FromEvent, (MilkPowder_Message_t **self, MilkPowder_Event_t *item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(dynamic_cast<MilkPowder::Message *>(milkpowder_cast(item)));
})

API_IMPL(MilkPowder_Message_FromMeta, (MilkPowder_Message_t **self, MilkPowder_Meta_t *item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(dynamic_cast<MilkPowder::Message *>(milkpowder_cast(item)));
})

API_IMPL(MilkPowder_Message_FromSysex, (MilkPowder_Message_t **self, MilkPowder_Sysex_t *item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(dynamic_cast<MilkPowder::Message *>(milkpowder_cast(item)));
})

API_IMPL(MilkPowder_Message_IsEvent, (const MilkPowder_Message_t *self, bool *item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *item = milkpowder_cast(self)->isEvent();
})

API_IMPL(MilkPowder_Message_IsMeta, (const MilkPowder_Message_t *self, bool *item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *item = milkpowder_cast(self)->isMeta();
})

API_IMPL(MilkPowder_Message_IsSysex, (const MilkPowder_Message_t *self, bool *item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *item = milkpowder_cast(self)->isSysex();
})

API_IMPL(MilkPowder_Message_ToEvent, (const MilkPowder_Message_t *self, const MilkPowder_Event_t **item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  if (!milkpowder_cast(self)->isEvent()) {
    throw MilkPowder::Except(MilkPowder::Except::Type::LogicError);
  }
  *item = milkpowder_cast(dynamic_cast<const MilkPowder::Event *>(milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Message_ToMeta, (const MilkPowder_Message_t *self, const MilkPowder_Meta_t **item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  if (!milkpowder_cast(self)->isMeta()) {
    throw MilkPowder::Except(MilkPowder::Except::Type::LogicError);
  }
  *item = milkpowder_cast(dynamic_cast<const MilkPowder::Meta *>(milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Message_ToSysex, (const MilkPowder_Message_t *self, const MilkPowder_Sysex_t **item), {
  if (self == nullptr || item == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  if (!milkpowder_cast(self)->isSysex()) {
    throw MilkPowder::Except(MilkPowder::Except::Type::LogicError);
  }
  *item = milkpowder_cast(dynamic_cast<const MilkPowder::Sysex *>(milkpowder_cast(self)));
})

// Event

API_IMPL(MilkPowder_Event_Parse, (MilkPowder_Event_t **self, const uint8_t *bytes, uint32_t length, uint32_t *size, uint8_t last), {
  if (self == nullptr || bytes == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  const uint8_t *cursor = bytes;
  auto ptr = MilkPowder::Event::Parse(cursor, bytes + length, last);
  if (size != nullptr) {
    *size = cursor - bytes;
  }
  *self = milkpowder_cast(ptr.release());
})

API_IMPL(MilkPowder_Event_Create, (MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(new MilkPowder::Event(delta, type, std::make_tuple(arg0, arg1)));
})

API_IMPL(MilkPowder_Event_Clone, (const MilkPowder_Event_t *self, MilkPowder_Event_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(new MilkPowder::Event(*milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Event_Destroy, (MilkPowder_Event_t *self), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
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

API_IMPL(MilkPowder_Meta_Parse, (MilkPowder_Meta_t **self, const uint8_t *bytes, uint32_t length, uint32_t *size), {
  if (self == nullptr || bytes == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  const uint8_t *cursor = bytes;
  auto ptr = MilkPowder::Meta::Parse(cursor, bytes + length);
  if (size != nullptr) {
    *size = cursor - bytes;
  }
  *self = milkpowder_cast(ptr.release());
})

API_IMPL(MilkPowder_Meta_Create, (MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t *args, uint32_t length), {
  if (self == nullptr || (length != 0 && args == nullptr)) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *self = milkpowder_cast(new MilkPowder::Meta(delta, type, std::vector<uint8_t>(args, args + length)));
})

API_IMPL(MilkPowder_Meta_Clone, (const MilkPowder_Meta_t *self, MilkPowder_Meta_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(new MilkPowder::Meta(*milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Meta_Destroy, (MilkPowder_Meta_t *self), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
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

API_IMPL(MilkPowder_Sysex_Parse, (MilkPowder_Sysex_t **self, const uint8_t *bytes, uint32_t length, uint32_t *size), {
  if (self == nullptr || bytes == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  const uint8_t *cursor = bytes;
  auto ptr = MilkPowder::Sysex::Parse(cursor, bytes + length);
  if (size != nullptr) {
    *size = cursor - bytes;
  }
  *self = milkpowder_cast(ptr.release());
})


using Vec = std::vector<std::tuple<uint32_t, std::vector<uint8_t>>>;

API_IMPL(MilkPowder_Sysex_Create, (MilkPowder_Sysex_t **self, uint32_t delta[], const uint8_t *args[], uint32_t length[], uint32_t size), {
  if (self == nullptr || (size != 0 && (delta == nullptr || args == nullptr || length == nullptr))) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  Vec vec(size);
  for (uint32_t i = 0; i < size; ++i) {
    vec.emplace_back(delta[i], std::vector<uint8_t>(args[i], args[i] + length[i]));
  }
  *self = milkpowder_cast(new MilkPowder::Sysex(std::move(vec)));
})

API_IMPL(MilkPowder_Sysex_Clone, (const MilkPowder_Sysex_t *self, MilkPowder_Sysex_t **another), {
  if (self == nullptr || another == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  *another = milkpowder_cast(new MilkPowder::Sysex(*milkpowder_cast(self)));
})

API_IMPL(MilkPowder_Sysex_Destroy, (MilkPowder_Sysex_t *self), {
  if (self == nullptr) {
    throw MilkPowder::Except(MilkPowder::Except::Type::NullPointer);
  }
  delete milkpowder_cast(self);
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
