#ifndef LIB_MILKPOWDER_CORE_H_
#define LIB_MILKPOWDER_CORE_H_

#include <milktea.h>

#define MilkPowder_API MilkTea_API

#ifdef __cplusplus
using MilkPowder_Logger_t = MilkTea_Logger_t;
using MilkPowder_LogLevel_t = MilkTea_LogLevel_t;
#else
typedef MilkTea_Logger_t MilkPowder_Logger_t;
typedef MilkTea_LogLevel_t MilkPowder_LogLevel_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

MilkPowder_API void
MilkPowder_Log_Init(MilkPowder_Logger_t config);

enum MilkPowder_Err_t {
  MilkPowder_Err_Nil,
  MilkPowder_Err_Unknown,
  MilkPowder_Err_Assertion,
  MilkPowder_Err_NullPointer,
  MilkPowder_Err_Unsupported,
  MilkPowder_Err_EndOfFile,
  MilkPowder_Err_InvalidParam,
  MilkPowder_Err_LogicError
};

#ifndef __cplusplus
typedef enum MilkPowder_Err_t MilkPowder_Err_t;
#endif

MilkPowder_API const char *
MilkPowder_Err_What();

inline const char *MilkPowder_Err_Name(MilkPowder_Err_t type) {
  switch (type) {
    case MilkPowder_Err_Nil: return "Nil";
    case MilkPowder_Err_Assertion: return "Assertion";
    case MilkPowder_Err_NullPointer: return "NullPointer";
    case MilkPowder_Err_Unsupported: return "Unsupported";
    case MilkPowder_Err_EndOfFile: return "EndOfFile";
    case MilkPowder_Err_InvalidParam: return "InvalidParam";
    case MilkPowder_Err_LogicError: return "LogicError";
    default: return "Unknown";
  }
}

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

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_Parse(MilkPowder_Midi_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_Create(MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_Clone(const MilkPowder_Midi_t *self, MilkPowder_Midi_t **another);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_Destroy(MilkPowder_Midi_t *self);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_GetFormat(const MilkPowder_Midi_t *self, uint16_t *format);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_GetNtrks(const MilkPowder_Midi_t *self, uint16_t *ntrks);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_GetDivision(const MilkPowder_Midi_t *self, uint16_t *division);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_GetTrack(const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Midi_Dump(const MilkPowder_Midi_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len));

// Track

MilkPowder_API MilkPowder_Err_t
MilkPowder_Track_Parse(MilkPowder_Track_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Err_t
MilkPowder_Track_Create(MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Track_Clone(const MilkPowder_Track_t *self, MilkPowder_Track_t **another);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Track_Destroy(MilkPowder_Track_t *self);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Track_GetMessages(const MilkPowder_Track_t *self, void *obj, void (*callback)(void *obj, const MilkPowder_Message_t *item));

MilkPowder_API MilkPowder_Err_t
MilkPowder_Track_Dump(const MilkPowder_Track_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len));

// Message

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_Parse(MilkPowder_Message_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_Clone(const MilkPowder_Message_t *self, MilkPowder_Message_t **another);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_Destroy(MilkPowder_Message_t *self);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_SetDelta(MilkPowder_Message_t *self, uint32_t delta);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_GetDelta(const MilkPowder_Message_t *self, uint32_t *delta);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_GetType(const MilkPowder_Message_t *self, uint8_t *type);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_FromEvent(MilkPowder_Message_t **self, MilkPowder_Event_t *item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_FromMeta(MilkPowder_Message_t **self, MilkPowder_Meta_t *item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_FromSysex(MilkPowder_Message_t **self, MilkPowder_Sysex_t *item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_IsEvent(const MilkPowder_Message_t *self, bool *item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_IsMeta(const MilkPowder_Message_t *self, bool *item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_IsSysex(const MilkPowder_Message_t *self, bool *item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_ToEvent(const MilkPowder_Message_t *self, const MilkPowder_Event_t **item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_ToMeta(const MilkPowder_Message_t *self, const MilkPowder_Meta_t **item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_ToSysex(const MilkPowder_Message_t *self, const MilkPowder_Sysex_t **item);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Message_Dump(const MilkPowder_Message_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len));

// Event

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_Parse(MilkPowder_Event_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_Create(MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_Clone(const MilkPowder_Event_t *self, MilkPowder_Event_t **another);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_Destroy(MilkPowder_Event_t *self);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_GetType(const MilkPowder_Event_t *self, uint8_t *type);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_GetArgs(const MilkPowder_Event_t *self, uint8_t *args);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Event_Dump(const MilkPowder_Event_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len));

// Meta

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_Parse(MilkPowder_Meta_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_Create(MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t *args, uint32_t length);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_Clone(const MilkPowder_Meta_t *self, MilkPowder_Meta_t **another);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_Destroy(MilkPowder_Meta_t *self);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_GetType(const MilkPowder_Meta_t *self, uint8_t *type);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_GetArgs(const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Meta_Dump(const MilkPowder_Meta_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len));

// Sysex

MilkPowder_API MilkPowder_Err_t
MilkPowder_Sysex_Parse(MilkPowder_Sysex_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Err_t
MilkPowder_Sysex_Create(MilkPowder_Sysex_t **self, uint32_t delta[], const uint8_t *args[], uint32_t length[], uint32_t size);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Sysex_Clone(const MilkPowder_Sysex_t *self, MilkPowder_Sysex_t **another);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Sysex_Destroy(MilkPowder_Sysex_t *self);

MilkPowder_API MilkPowder_Err_t
MilkPowder_Sysex_GetArgs(const MilkPowder_Sysex_t *self, void *obj, void (*callback)(void *obj, uint32_t delta, const uint8_t *args, uint32_t length));

MilkPowder_API MilkPowder_Err_t
MilkPowder_Sysex_Dump(const MilkPowder_Sysex_t *self, void *obj, void (*callback)(void *obj, const uint8_t *bytes, size_t len));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKPOWDER_CORE_H_
