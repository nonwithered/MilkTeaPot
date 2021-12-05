#ifndef LIB_MILKPOWDER_COMMON_MAPPING_H_
#define LIB_MILKPOWDER_COMMON_MAPPING_H_

#include <milkpowder/core.h>

namespace MilkPowder {

namespace Mapping {

struct Midi {
  using raw_type = MilkPowder_Midi_t;
  static constexpr auto raw_create = MilkPowder_Midi_Create;
  static constexpr auto raw_destroy = MilkPowder_Midi_Destroy;
  static constexpr auto raw_clone = MilkPowder_Midi_Clone;
  static constexpr auto raw_parse = MilkPowder_Midi_Parse;
  static constexpr auto raw_dump = MilkPowder_Midi_Dump;
  static constexpr auto raw_get_format = MilkPowder_Midi_GetFormat;
  static constexpr auto raw_get_ntrks = MilkPowder_Midi_GetNtrks;
  static constexpr auto raw_get_division = MilkPowder_Midi_GetDivision;
  static constexpr auto raw_get_track = MilkPowder_Midi_GetTrack;
};

struct Track {
  using raw_type = MilkPowder_Track_t;
  static constexpr auto raw_create = MilkPowder_Track_Create;
  static constexpr auto raw_destroy = MilkPowder_Track_Destroy;
  static constexpr auto raw_clone = MilkPowder_Track_Clone;
  static constexpr auto raw_parse = MilkPowder_Track_Parse;
  static constexpr auto raw_dump = MilkPowder_Track_Dump;
  static constexpr auto raw_get_messages = MilkPowder_Track_GetMessages;
};

struct Message {
  using raw_type = MilkPowder_Message_t;
  static constexpr auto raw_destroy = MilkPowder_Message_Destroy;
  static constexpr auto raw_clone = MilkPowder_Message_Clone;
  static constexpr auto raw_parse = MilkPowder_Message_Parse;
  static constexpr auto raw_dump = MilkPowder_Message_Dump;
  static constexpr auto raw_get_type = MilkPowder_Message_GetType;
  static constexpr auto raw_get_delta = MilkPowder_Message_GetDelta;
  static constexpr auto raw_set_delta = MilkPowder_Message_SetDelta;
};

struct Event {
  using raw_type = MilkPowder_Event_t;
  static constexpr auto raw_create = MilkPowder_Event_Create;
  static constexpr auto raw_destroy = MilkPowder_Event_Destroy;
  static constexpr auto raw_clone = MilkPowder_Event_Clone;
  static constexpr auto raw_parse = MilkPowder_Event_Parse;
  static constexpr auto raw_dump = MilkPowder_Event_Dump;
  static constexpr auto raw_get_type = MilkPowder_Event_GetType;
  static constexpr auto raw_get_args = MilkPowder_Event_GetArgs;
  static constexpr auto raw_message_from = MilkPowder_Message_FromEvent;
  static constexpr auto raw_message_is = MilkPowder_Message_IsEvent;
  static constexpr auto raw_message_to = MilkPowder_Message_ToEvent;
};

struct Meta {
  using raw_type = MilkPowder_Meta_t;
  static constexpr auto raw_create = MilkPowder_Meta_Create;
  static constexpr auto raw_destroy = MilkPowder_Meta_Destroy;
  static constexpr auto raw_clone = MilkPowder_Meta_Clone;
  static constexpr auto raw_parse = MilkPowder_Meta_Parse;
  static constexpr auto raw_dump = MilkPowder_Meta_Dump;
  static constexpr auto raw_get_type = MilkPowder_Meta_GetType;
  static constexpr auto raw_get_args = MilkPowder_Meta_GetArgs;
  static constexpr auto raw_message_from = MilkPowder_Message_FromMeta;
  static constexpr auto raw_message_is = MilkPowder_Message_IsMeta;
  static constexpr auto raw_message_to = MilkPowder_Message_ToMeta;
};

struct Sysex {
  using raw_type = MilkPowder_Sysex_t;
  static constexpr auto raw_create = MilkPowder_Sysex_Create;
  static constexpr auto raw_destroy = MilkPowder_Sysex_Destroy;
  static constexpr auto raw_clone = MilkPowder_Sysex_Clone;
  static constexpr auto raw_parse = MilkPowder_Sysex_Parse;
  static constexpr auto raw_dump = MilkPowder_Sysex_Dump;
  static constexpr auto raw_get_args = MilkPowder_Sysex_GetArgs;
  static constexpr auto raw_message_from = MilkPowder_Message_FromSysex;
  static constexpr auto raw_message_is = MilkPowder_Message_IsSysex;
  static constexpr auto raw_message_to = MilkPowder_Message_ToSysex;
};

template<typename T>
struct FromRawType;

template<>
struct FromRawType<MilkPowder_Midi_t> {
  using target = Midi;
};

template<>
struct FromRawType<MilkPowder_Track_t> {
  using target = Track;
};

template<>
struct FromRawType<MilkPowder_Message_t> {
  using target = Message;
};

template<>
struct FromRawType<MilkPowder_Event_t> {
  using target = Event;
};

template<>
struct FromRawType<MilkPowder_Meta_t> {
  using target = Meta;
};

template<>
struct FromRawType<MilkPowder_Sysex_t> {
  using target = Sysex;
};

} // namespace Mapping
} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_COMMON_MAPPING_H_
