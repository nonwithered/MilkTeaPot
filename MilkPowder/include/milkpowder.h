#ifndef LIB_MILKPOWDER_H_
#define LIB_MILKPOWDER_H_

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkPowder_API __declspec(dllexport)
#else
#define MilkPowder_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdbool.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum MilkPowder_Log_Level_t {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT
};

struct MilkPowder_Log_Config_t {
  void *obj;
  void (*debug)(void *obj, const char *tag, const char *msg);
  void (*info)(void *obj, const char *tag, const char *msg);
  void (*warn)(void *obj, const char *tag, const char *msg);
  void (*error)(void *obj, const char *tag, const char *msg);
  enum MilkPowder_Log_Level_t level;
};

enum MilkPowder_Errno_t {
  Nil,
  Unknown,
  Assertion,
  NullPointer,
  Unsupported,
  EndOfFile,
  InvalidParam,
  LogicError
};

struct MilkPowder_Midi_t;
struct MilkPowder_Track_t;
struct MilkPowder_Message_t;
struct MilkPowder_Event_t;
struct MilkPowder_Meta_t;
struct MilkPowder_Sysex_t;

#ifndef __cplusplus
typedef enum MilkPowder_Log_Level_t MilkPowder_Log_Level_t;
typedef enum MilkPowder_Log_Config_t MilkPowder_Log_Config_t;
typedef enum MilkPowder_Errno_t MilkPowder_Errno_t;
typedef enum MilkPowder_Midi_t MilkPowder_Midi_t;
typedef enum MilkPowder_Track_t MilkPowder_Track_t;
typedef enum MilkPowder_Message_t MilkPowder_Message_t;
typedef enum MilkPowder_Event_t MilkPowder_Event_t;
typedef enum MilkPowder_Meta_t MilkPowder_Meta_t;
typedef enum MilkPowder_Sysex_t MilkPowder_Sysex_t;
#endif

MilkPowder_API void
MilkPowder_Log_Init(MilkPowder_Log_Config_t config);

// Midi

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_Parse(MilkPowder_Midi_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_Create(MilkPowder_Midi_t **self, uint16_t format, uint16_t ntrks, uint16_t division, MilkPowder_Track_t *items[]);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_Clone(const MilkPowder_Midi_t *self, MilkPowder_Midi_t **another);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_Destroy(MilkPowder_Midi_t *self);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_GetFormat(const MilkPowder_Midi_t *self, uint16_t *format);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_GetNtrks(const MilkPowder_Midi_t *self, uint16_t *ntrks);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_GetDivision(const MilkPowder_Midi_t *self, uint16_t *division);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Midi_GetTrack(const MilkPowder_Midi_t *self, uint16_t index, const MilkPowder_Track_t **item);

// Track

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Track_Parse(MilkPowder_Track_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Track_Create(MilkPowder_Track_t **self, MilkPowder_Message_t *items[], uint32_t length);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Track_Clone(const MilkPowder_Track_t *self, MilkPowder_Track_t **another);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Track_Destroy(MilkPowder_Track_t *self);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Track_GetMessages(const MilkPowder_Track_t *self, void *obj, void (*callback)(void *obj, const MilkPowder_Message_t *item));

// Message

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_Parse(MilkPowder_Message_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_Clone(const MilkPowder_Message_t *self, MilkPowder_Message_t **another);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_Destroy(MilkPowder_Message_t *self);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_GetDelta(const MilkPowder_Message_t *self, uint32_t *delta);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_FromEvent(MilkPowder_Message_t **self, MilkPowder_Event_t *item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_FromMeta(MilkPowder_Message_t **self, MilkPowder_Meta_t *item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_FromSysex(MilkPowder_Message_t **self, MilkPowder_Sysex_t *item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_IsEvent(const MilkPowder_Message_t *self, bool *item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_IsMeta(const MilkPowder_Message_t *self, bool *item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_IsSysex(const MilkPowder_Message_t *self, bool *item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_ToEvent(const MilkPowder_Message_t *self, const MilkPowder_Event_t **item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_ToMeta(const MilkPowder_Message_t *self, const MilkPowder_Meta_t **item);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Message_ToSysex(const MilkPowder_Message_t *self, const MilkPowder_Sysex_t **item);

// Event

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Event_Parse(MilkPowder_Event_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte), uint8_t last);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Event_Create(MilkPowder_Event_t **self, uint32_t delta, uint8_t type, uint8_t arg0, uint8_t arg1);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Event_Clone(const MilkPowder_Event_t *self, MilkPowder_Event_t **another);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Event_Destroy(MilkPowder_Event_t *self);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Event_GetType(const MilkPowder_Event_t *self, uint8_t *type);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Event_GetArgs(const MilkPowder_Event_t *self, uint8_t *args);

// Meta

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Meta_Parse(MilkPowder_Meta_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Meta_Create(MilkPowder_Meta_t **self, uint32_t delta, uint8_t type, const uint8_t *args, uint32_t length);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Meta_Clone(const MilkPowder_Meta_t *self, MilkPowder_Meta_t **another);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Meta_Destroy(MilkPowder_Meta_t *self);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Meta_GetType(const MilkPowder_Meta_t *self, uint8_t *type);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Meta_GetArgs(const MilkPowder_Meta_t *self, const uint8_t **args, uint32_t *length);

// Sysex

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Sysex_Parse(MilkPowder_Sysex_t **self, void *obj, bool (*callback)(void *obj, uint8_t *byte));

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Sysex_Create(MilkPowder_Sysex_t **self, uint32_t delta[], const uint8_t *args[], uint32_t length[], uint32_t size);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Sysex_Clone(const MilkPowder_Sysex_t *self, MilkPowder_Sysex_t **another);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Sysex_Destroy(MilkPowder_Sysex_t *self);

MilkPowder_API MilkPowder_Errno_t
MilkPowder_Sysex_GetArgs(const MilkPowder_Sysex_t *self, void *obj, void (*callback)(void *obj, uint32_t delta, const uint8_t *args, uint32_t length));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKPOWDER_H_
