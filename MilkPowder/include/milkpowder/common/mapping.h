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
  static constexpr auto raw_set_format = MilkPowder_Midi_SetFormat;
  static constexpr auto raw_get_ntrks = MilkPowder_Midi_GetNtrks;
  static constexpr auto raw_get_division = MilkPowder_Midi_GetDivision;
  static constexpr auto raw_set_division = MilkPowder_Midi_SetDivision;
  static constexpr auto raw_get_track = MilkPowder_Midi_GetTrack;
  static constexpr auto raw_all_track = MilkPowder_Midi_AllTrack;
};

struct Track {
  using raw_type = MilkPowder_Track_t;
  using raw_consumer_type = MilkPowder_Track_Consumer_t;
  static constexpr auto raw_create = MilkPowder_Track_Create;
  static constexpr auto raw_destroy = MilkPowder_Track_Destroy;
  static constexpr auto raw_clone = MilkPowder_Track_Clone;
  static constexpr auto raw_parse = MilkPowder_Track_Parse;
  static constexpr auto raw_dump = MilkPowder_Track_Dump;
  static constexpr auto raw_get_count = MilkPowder_Track_GetCount;
  static constexpr auto raw_get_message = MilkPowder_Track_GetMessage;
  static constexpr auto raw_all_message = MilkPowder_Track_AllMessage;
};

struct Message {
  using raw_type = MilkPowder_Message_t;
  using raw_consumer_type = MilkPowder_Message_Consumer_t;
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
  static constexpr auto raw_from_message = MilkPowder_Event_FromMessage;
  static constexpr auto raw_to_message = MilkPowder_Event_ToMessage;
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
  static constexpr auto raw_from_message = MilkPowder_Meta_FromMessage;
  static constexpr auto raw_to_message = MilkPowder_Meta_ToMessage;
  static constexpr auto raw_message_from = MilkPowder_Message_FromMeta;
  static constexpr auto raw_message_is = MilkPowder_Message_IsMeta;
  static constexpr auto raw_message_to = MilkPowder_Message_ToMeta;
};

struct Sysex {
  using raw_type = MilkPowder_Sysex_t;
  using raw_item_type = MilkPowder_Sysex_Item_t;
  using raw_item_mut_type = MilkPowder_Sysex_Item_mut_t;
  using raw_consumer_type = MilkPowder_Sysex_Item_Consumer_t;
  static constexpr auto raw_create = MilkPowder_Sysex_Create;
  static constexpr auto raw_destroy = MilkPowder_Sysex_Destroy;
  static constexpr auto raw_clone = MilkPowder_Sysex_Clone;
  static constexpr auto raw_parse = MilkPowder_Sysex_Parse;
  static constexpr auto raw_dump = MilkPowder_Sysex_Dump;
  static constexpr auto raw_get_count = MilkPowder_Sysex_GetCount;
  static constexpr auto raw_get_item = MilkPowder_Sysex_GetItem;
  static constexpr auto raw_all_item = MilkPowder_Sysex_AllItem;
  static constexpr auto raw_from_message = MilkPowder_Sysex_FromMessage;
  static constexpr auto raw_to_message = MilkPowder_Sysex_ToMessage;
  static constexpr auto raw_message_from = MilkPowder_Message_FromSysex;
  static constexpr auto raw_message_is = MilkPowder_Message_IsSysex;
  static constexpr auto raw_message_to = MilkPowder_Message_ToSysex;
};

class ByteReader final {
  static constexpr char TAG[] = "MilkPowder::Mapping::ByteReader";
  using raw_type = MilkPowder_ByteReader_t;
  using call_type = std::function<size_t(uint8_t [], size_t)>;
 public:
  ByteReader(call_type call) {
    call_ = [call](uint8_t bytes[], size_t *len) -> MilkTea_Exception_t MilkTea_with_except({
      MilkTea_nonnull(bytes);
      MilkTea_nonnull(len);
      *len = call(bytes, *len);
    });
  }
  operator raw_type(){
    return MilkTea::FunctionAdapter::ToRawType<raw_type>(call_);
  }
 private:
  std::function<MilkTea_Exception_t(uint8_t [], size_t *)> call_;
};

class ByteWriter final {
  static constexpr char TAG[] = "MilkPowder::Mapping::ByteWriter";
  using raw_type = MilkPowder_ByteWriter_t;
  using call_type = std::function<void(const uint8_t [], size_t)>;
 public:
  ByteWriter(call_type call) {
    call_ = [call](const uint8_t bytes[], size_t len) -> MilkTea_Exception_t MilkTea_with_except({
      MilkTea_nonnull(bytes);
      call(bytes, len);
    });
  }
  operator raw_type(){
    return MilkTea::FunctionAdapter::ToRawType<raw_type>(call_);
  }
 private:
  std::function<MilkTea_Exception_t(const uint8_t [], size_t)> call_;
};

} // namespace Mapping
} // namespace MilkPowder

#endif // ifndef LIB_MILKPOWDER_COMMON_MAPPING_H_
