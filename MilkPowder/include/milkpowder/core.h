#ifndef LIB_MILKPOWDER_CORE_H_
#define LIB_MILKPOWDER_CORE_H_

#include <milktea.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MilkPowder_Midi_t;
struct MilkPowder_Track_t;
struct MilkPowder_Message_t;
struct MilkPowder_Event_t;
struct MilkPowder_Meta_t;
struct MilkPowder_Sysex_t;

#ifndef __cplusplus
typedef struct MilkPowder_Midi_t MilkPowder_Midi_t;
typedef struct MilkPowder_Track_t MilkPowder_Track_t;
typedef struct MilkPowder_Message_t MilkPowder_Message_t;
typedef struct MilkPowder_Event_t MilkPowder_Event_t;
typedef struct MilkPowder_Meta_t MilkPowder_Meta_t;
typedef struct MilkPowder_Sysex_t MilkPowder_Sysex_t;
#endif

struct MilkPowder_Sysex_Item_t {
  uint32_t delta_;
  uint32_t length_;
  const uint8_t *args_;
};

#ifndef __cplusplus
typedef struct MilkPowder_Sysex_Item_t MilkPowder_Sysex_Item_t;
#endif

struct MilkPowder_Sysex_Item_mut_t {
  uint32_t *delta_;
  uint32_t length_;
  uint8_t *args_;
};

#ifndef __cplusplus
typedef struct MilkPowder_Sysex_Item_mut_t MilkPowder_Sysex_Item_mut_t;
#endif

MilkTea_Function_t(MilkPowder_ByteReader_t, MilkTea_Exception_t, uint8_t [], size_t *)

#ifndef __cplusplus
typedef struct MilkPowder_ByteReader_t MilkPowder_ByteReader_t;
#endif

MilkTea_Function_t(MilkPowder_ByteWriter_t, MilkTea_Exception_t, const uint8_t [], size_t)

#ifndef __cplusplus
typedef struct MilkPowder_ByteWriter_t MilkPowder_ByteWriter_t;
#endif

MilkTea_Consumer_t(MilkPowder_Track_Consumer_t, MilkPowder_Track_t *)

#ifndef __cplusplus
typedef struct MilkPowder_Track_Consumer_t MilkPowder_Track_Consumer_t;
#endif

MilkTea_Consumer_t(MilkPowder_Message_Consumer_t, MilkPowder_Message_t *)

#ifndef __cplusplus
typedef struct MilkPowder_Message_Consumer_t MilkPowder_Message_Consumer_t;
#endif

MilkTea_Consumer_t(MilkPowder_Sysex_Item_Consumer_t, MilkPowder_Sysex_Item_mut_t)

#ifndef __cplusplus
typedef struct MilkPowder_Sysex_Item_Consumer_t MilkPowder_Sysex_Item_Consumer_t;
#endif

// Midi

MilkTea_api
MilkPowder_Midi_Parse(MilkPowder_Midi_t **obj, MilkPowder_ByteReader_t reader);

MilkTea_api
MilkPowder_Midi_Create(MilkPowder_Midi_t **obj, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]);

MilkTea_api
MilkPowder_Midi_Clone(const MilkPowder_Midi_t *obj, MilkPowder_Midi_t **another);

MilkTea_api
MilkPowder_Midi_Destroy(MilkPowder_Midi_t *obj);

MilkTea_api
MilkPowder_Midi_GetFormat(const MilkPowder_Midi_t *obj, uint16_t *format);

MilkTea_api
MilkPowder_Midi_SetFormat(MilkPowder_Midi_t *obj, uint16_t format);

MilkTea_api
MilkPowder_Midi_GetNtrks(const MilkPowder_Midi_t *obj, uint16_t *ntrks);

MilkTea_api
MilkPowder_Midi_GetDivision(const MilkPowder_Midi_t *obj, uint16_t *division);

MilkTea_api
MilkPowder_Midi_SetDivision(MilkPowder_Midi_t *obj, uint16_t division);

MilkTea_api
MilkPowder_Midi_GetTrack(const MilkPowder_Midi_t *obj, uint16_t index, const MilkPowder_Track_t **item);

MilkTea_api
MilkPowder_Midi_AllTrack(MilkPowder_Midi_t *obj, MilkPowder_Track_Consumer_t consumer);

MilkTea_api
MilkPowder_Midi_Dump(const MilkPowder_Midi_t *obj, MilkPowder_ByteWriter_t writer);

// Track

MilkTea_api
MilkPowder_Track_Parse(MilkPowder_Track_t **obj, MilkPowder_ByteReader_t reader);

MilkTea_api
MilkPowder_Track_Create(MilkPowder_Track_t **obj, MilkPowder_Message_t *items[], uint32_t length);

MilkTea_api
MilkPowder_Track_Clone(const MilkPowder_Track_t *obj, MilkPowder_Track_t **another);

MilkTea_api
MilkPowder_Track_Destroy(MilkPowder_Track_t *obj);

MilkTea_api
MilkPowder_Track_GetCount(const MilkPowder_Track_t *obj, uint32_t *count);

MilkTea_api
MilkPowder_Track_GetMessage(const MilkPowder_Track_t *obj, uint32_t index, const MilkPowder_Message_t **item);

MilkTea_api
MilkPowder_Track_AllMessage(MilkPowder_Track_t *obj, MilkPowder_Message_Consumer_t consumer);

MilkTea_api
MilkPowder_Track_Dump(const MilkPowder_Track_t *obj, MilkPowder_ByteWriter_t writer);

// Message

MilkTea_api
MilkPowder_Message_Parse(MilkPowder_Message_t **obj, MilkPowder_ByteReader_t reader, uint8_t last);

MilkTea_api
MilkPowder_Message_Clone(const MilkPowder_Message_t *obj, MilkPowder_Message_t **another);

MilkTea_api
MilkPowder_Message_Destroy(MilkPowder_Message_t *obj);

MilkTea_api
MilkPowder_Message_SetDelta(MilkPowder_Message_t *obj, uint32_t delta);

MilkTea_api
MilkPowder_Message_GetDelta(const MilkPowder_Message_t *obj, uint32_t *delta);

MilkTea_api
MilkPowder_Message_GetType(const MilkPowder_Message_t *obj, uint8_t *type);

MilkTea_api
MilkPowder_Message_Dump(const MilkPowder_Message_t *obj, MilkPowder_ByteWriter_t writer);

MilkTea_api
MilkPowder_Message_FromEvent(MilkPowder_Message_t **obj, MilkPowder_Event_t *item);

MilkTea_api
MilkPowder_Message_FromMeta(MilkPowder_Message_t **obj, MilkPowder_Meta_t *item);

MilkTea_api
MilkPowder_Message_FromSysex(MilkPowder_Message_t **obj, MilkPowder_Sysex_t *item);

MilkTea_api
MilkPowder_Message_IsEvent(const MilkPowder_Message_t *obj, bool *item);

MilkTea_api
MilkPowder_Message_IsMeta(const MilkPowder_Message_t *obj, bool *item);

MilkTea_api
MilkPowder_Message_IsSysex(const MilkPowder_Message_t *obj, bool *item);

MilkTea_api
MilkPowder_Message_ToEvent(const MilkPowder_Message_t *obj, const MilkPowder_Event_t **item);

MilkTea_api
MilkPowder_Message_ToMeta(const MilkPowder_Message_t *obj, const MilkPowder_Meta_t **item);

MilkTea_api
MilkPowder_Message_ToSysex(const MilkPowder_Message_t *obj, const MilkPowder_Sysex_t **item);

// Event

MilkTea_api
MilkPowder_Event_Parse(MilkPowder_Event_t **obj, MilkPowder_ByteReader_t reader, uint8_t last);

MilkTea_api
MilkPowder_Event_Create(MilkPowder_Event_t **obj, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1);

MilkTea_api
MilkPowder_Event_Clone(const MilkPowder_Event_t *obj, MilkPowder_Event_t **another);

MilkTea_api
MilkPowder_Event_Destroy(MilkPowder_Event_t *obj);

MilkTea_api
MilkPowder_Event_GetType(const MilkPowder_Event_t *obj, uint8_t *type);

MilkTea_api
MilkPowder_Event_GetArgs(const MilkPowder_Event_t *obj, uint8_t *args);

MilkTea_api
MilkPowder_Event_Dump(const MilkPowder_Event_t *obj, MilkPowder_ByteWriter_t writer);

MilkTea_api
MilkPowder_Event_FromMessage(MilkPowder_Event_t **obj, MilkPowder_Message_t *item);

MilkTea_api
MilkPowder_Event_ToMessage(const MilkPowder_Event_t *obj, const MilkPowder_Message_t **item);

// Meta

MilkTea_api
MilkPowder_Meta_Parse(MilkPowder_Meta_t **obj, MilkPowder_ByteReader_t reader);

MilkTea_api
MilkPowder_Meta_Create(MilkPowder_Meta_t **obj, uint32_t delta, uint8_t type, const uint8_t args[], uint32_t length);

MilkTea_api
MilkPowder_Meta_Clone(const MilkPowder_Meta_t *obj, MilkPowder_Meta_t **another);

MilkTea_api
MilkPowder_Meta_Destroy(MilkPowder_Meta_t *obj);

MilkTea_api
MilkPowder_Meta_GetType(const MilkPowder_Meta_t *obj, uint8_t *type);

MilkTea_api
MilkPowder_Meta_GetArgs(const MilkPowder_Meta_t *obj, const uint8_t **args, uint32_t *length);

MilkTea_api
MilkPowder_Meta_Dump(const MilkPowder_Meta_t *obj, MilkPowder_ByteWriter_t writer);

MilkTea_api
MilkPowder_Meta_FromMessage(MilkPowder_Meta_t **obj, MilkPowder_Message_t *item);

MilkTea_api
MilkPowder_Meta_ToMessage(const MilkPowder_Meta_t *obj, const MilkPowder_Message_t **item);

// Sysex

MilkTea_api
MilkPowder_Sysex_Parse(MilkPowder_Sysex_t **obj, MilkPowder_ByteReader_t reader);

MilkTea_api
MilkPowder_Sysex_Create(MilkPowder_Sysex_t **obj, uint32_t size, const MilkPowder_Sysex_Item_t items[]);

MilkTea_api
MilkPowder_Sysex_Clone(const MilkPowder_Sysex_t *obj, MilkPowder_Sysex_t **another);

MilkTea_api
MilkPowder_Sysex_Destroy(MilkPowder_Sysex_t *obj);

MilkTea_api
MilkPowder_Sysex_GetCount(const MilkPowder_Sysex_t *obj, uint32_t *count);

MilkTea_api
MilkPowder_Sysex_GetItem(const MilkPowder_Sysex_t *obj, uint32_t index, MilkPowder_Sysex_Item_t *item);

MilkTea_api
MilkPowder_Sysex_AllItem(MilkPowder_Sysex_t *obj, MilkPowder_Sysex_Item_Consumer_t consumer);

MilkTea_api
MilkPowder_Sysex_Dump(const MilkPowder_Sysex_t *obj, MilkPowder_ByteWriter_t writer);

MilkTea_api
MilkPowder_Sysex_FromMessage(MilkPowder_Sysex_t **obj, MilkPowder_Message_t *item);

MilkTea_api
MilkPowder_Sysex_ToMessage(const MilkPowder_Sysex_t *obj, const MilkPowder_Message_t **item);

// SoyBean

struct SoyBean_Handle_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Deleter)(void *obj);
  MilkTea_Exception_t (MilkTea_call *SendMessage)(void *obj, uint8_t type, uint8_t arg0, uint8_t arg1);
};

#ifndef __cplusplus
typedef struct SoyBean_Handle_Interface_t SoyBean_Handle_Interface_t;
#endif

struct SoyBean_Handle_t {
  void *obj_;
  const SoyBean_Handle_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyBean_Handle_t SoyBean_Handle_t;
#endif

struct SoyBean_Factory_Interface_t {
  MilkTea_Exception_t (MilkTea_call *Deleter)(void *obj);
  MilkTea_Exception_t (MilkTea_call *Create)(void *obj, SoyBean_Handle_t *handle);
};

#ifndef __cplusplus
typedef struct SoyBean_Factory_Interface_t SoyBean_Factory_Interface_t;
#endif

struct SoyBean_Factory_t {
  void *obj_;
  const SoyBean_Factory_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyBean_Factory_t SoyBean_Factory_t;
#endif

MilkTea_api
SoyBean_Handle_Create(SoyBean_Handle_t *obj, SoyBean_Factory_t factory);

MilkTea_api
SoyBean_Handle_Destroy(SoyBean_Handle_t obj);

MilkTea_api
SoyBean_Handle_SendMessage(SoyBean_Handle_t obj, uint8_t type, uint8_t arg0, uint8_t arg1);

MilkTea_api
SoyBean_Factory_Destroy(SoyBean_Factory_t obj);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKPOWDER_CORE_H_
