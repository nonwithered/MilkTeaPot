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

// Midi

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_Parse(MilkPowder_Midi_t **self, void *obj, bool (MilkTea_call *callback)(void *obj, uint8_t *byte));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_Create(MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_Clone(const MilkPowder_Midi_t *self, MilkPowder_Midi_t **another);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_Destroy(MilkPowder_Midi_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_GetFormat(const MilkPowder_Midi_t *self, uint16_t *format);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_GetNtrks(const MilkPowder_Midi_t *self, uint16_t *ntrks);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_GetDivision(const MilkPowder_Midi_t *self, uint16_t *division);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_GetTrack(const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Midi_Dump(const MilkPowder_Midi_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const uint8_t *bytes, size_t len));

// Track

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Track_Parse(MilkPowder_Track_t **self, void *obj, bool (MilkTea_call *callback)(void *obj, uint8_t *byte));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Track_Create(MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Track_Clone(const MilkPowder_Track_t *self, MilkPowder_Track_t **another);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Track_Destroy(MilkPowder_Track_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Track_GetMessages(const MilkPowder_Track_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const MilkPowder_Message_t *item));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Track_Dump(const MilkPowder_Track_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const uint8_t *bytes, size_t len));

// Message

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_Parse(MilkPowder_Message_t **self, void *obj, bool (MilkTea_call *callback)(void *obj, uint8_t *byte), uint8_t last);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_Clone(const MilkPowder_Message_t *self, MilkPowder_Message_t **another);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_Destroy(MilkPowder_Message_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_SetDelta(MilkPowder_Message_t *self, uint32_t delta);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_GetDelta(const MilkPowder_Message_t *self, uint32_t *delta);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_GetType(const MilkPowder_Message_t *self, uint8_t *type);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_Dump(const MilkPowder_Message_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const uint8_t *bytes, size_t len));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_FromEvent(MilkPowder_Message_t **self, MilkPowder_Event_t *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_FromMeta(MilkPowder_Message_t **self, MilkPowder_Meta_t *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_FromSysex(MilkPowder_Message_t **self, MilkPowder_Sysex_t *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_IsEvent(const MilkPowder_Message_t *self, bool *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_IsMeta(const MilkPowder_Message_t *self, bool *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_IsSysex(const MilkPowder_Message_t *self, bool *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_ToEvent(const MilkPowder_Message_t *self, const MilkPowder_Event_t **item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_ToMeta(const MilkPowder_Message_t *self, const MilkPowder_Meta_t **item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Message_ToSysex(const MilkPowder_Message_t *self, const MilkPowder_Sysex_t **item);

// Event

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_Parse(MilkPowder_Event_t **self, void *obj, bool (MilkTea_call *callback)(void *obj, uint8_t *byte), uint8_t last);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_Create(MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_Clone(const MilkPowder_Event_t *self, MilkPowder_Event_t **another);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_Destroy(MilkPowder_Event_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_GetType(const MilkPowder_Event_t *self, uint8_t *type);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_GetArgs(const MilkPowder_Event_t *self, uint8_t *args);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_Dump(const MilkPowder_Event_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const uint8_t *bytes, size_t len));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_FromMessage(MilkPowder_Event_t **self, MilkPowder_Message_t *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Event_ToMessage(const MilkPowder_Event_t *self, const MilkPowder_Message_t **item);

// Meta

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_Parse(MilkPowder_Meta_t **self, void *obj, bool (MilkTea_call *callback)(void *obj, uint8_t *byte));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_Create(MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t args[], uint32_t length);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_Clone(const MilkPowder_Meta_t *self, MilkPowder_Meta_t **another);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_Destroy(MilkPowder_Meta_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_GetType(const MilkPowder_Meta_t *self, uint8_t *type);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_GetArgs(const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_Dump(const MilkPowder_Meta_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const uint8_t *bytes, size_t len));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_FromMessage(MilkPowder_Meta_t **self, MilkPowder_Message_t *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Meta_ToMessage(const MilkPowder_Meta_t *self, const MilkPowder_Message_t **item);

// Sysex

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_Parse(MilkPowder_Sysex_t **self, void *obj, bool (MilkTea_call *callback)(void *obj, uint8_t *byte));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_Create(MilkPowder_Sysex_t **self, uint32_t delta[], const uint8_t *const args[], const uint32_t length[], uint32_t size);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_Clone(const MilkPowder_Sysex_t *self, MilkPowder_Sysex_t **another);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_Destroy(MilkPowder_Sysex_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_GetArgs(const MilkPowder_Sysex_t *self, void *obj, void (MilkTea_call *callback)(void *obj, uint32_t delta, const uint8_t *args, uint32_t length));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_Dump(const MilkPowder_Sysex_t *self, void *obj, void (MilkTea_call *callback)(void *obj, const uint8_t *bytes, size_t len));

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_FromMessage(MilkPowder_Sysex_t **self, MilkPowder_Message_t *item);

MilkTea_api MilkTea_Exception_t MilkTea_call
MilkPowder_Sysex_ToMessage(const MilkPowder_Sysex_t *self, const MilkPowder_Message_t **item);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKPOWDER_CORE_H_
