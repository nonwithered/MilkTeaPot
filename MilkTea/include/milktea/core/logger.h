#ifndef LIB_MILKTEA_CORE_LOGGER_H_
#define LIB_MILKTEA_CORE_LOGGER_H_

#include <milktea/core/except.h>

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_Logger_Level_t {
  MilkTea_Logger_Level_DEBUG,
  MilkTea_Logger_Level_INFO,
  MilkTea_Logger_Level_WARN,
  MilkTea_Logger_Level_ERROR,
  MilkTea_Logger_Level_ASSERT
};

#ifndef __cplusplus
typedef enum MilkTea_Logger_Level_t MilkTea_Logger_Level_t;
#endif // ifndef __cplusplus

struct MilkTea_Logger_Interface_t {
  void (MilkTea_call *Deleter)(void *self);
  void (MilkTea_call *Debug)(void *self, const char *tag, const char *msg);
  void (MilkTea_call *Info)(void *self, const char *tag, const char *msg);
  void (MilkTea_call *Warn)(void *self, const char *tag, const char *msg);
  void (MilkTea_call *Error)(void *self, const char *tag, const char *msg);
};

#ifndef __cplusplus
typedef struct MilkTea_Logger_Interface_t MilkTea_Logger_Interface_t;
#endif // ifndef __cplusplus

struct MilkTea_Logger_t {
  void *self_;
  enum MilkTea_Logger_Level_t level_;
  const MilkTea_Logger_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct MilkTea_Logger_t MilkTea_Logger_t;
#endif // ifndef __cplusplus

MilkTea_decl(bool)
MilkTea_Logger_Config(MilkTea_Logger_t logger);

MilkTea_decl(MilkTea_Logger_Level_t)
MilkTea_Logger_GetLevel();

MilkTea_decl(void)
MilkTea_Logger_Print(MilkTea_Logger_Level_t level, const char *tag, const char *msg);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include <cstdio>
#include <cstddef>
#else // ifdef __cplusplus
#include <stdio.h>
#include <stddef.h>
#endif // ifdef __cplusplus

#ifndef MilkTea_log_msg_size
#define MilkTea_log_msg_size 1024
#endif // ifndef MilkTea_log_msg_size

#ifdef NDEBUG
#define MilkTea_ignore_log
#endif // ifdef NDEBUG

#ifdef MilkTea_ignore_log
#define MilkTea_log(...) MilkTea_block({})
#else // ifdef MilkTea_ignore_log
#define MilkTea_log(LEVEL, TAG, ...) MilkTea_block({ \
  MilkTea_Logger_Level_t level_ = MilkTea_Logger_GetLevel(); \
  if (level_ > MilkTea_Logger_Level_##LEVEL) { \
    break; \
  } \
  char msg_[MilkTea_log_msg_size]; \
  snprintf(msg_, MilkTea_log_msg_size, ##__VA_ARGS__); \
  MilkTea_Logger_Print(MilkTea_Logger_Level_##LEVEL, TAG, msg_); \
})
#endif // ifdef MilkTea_ignore_log

#define MilkTea_logD(...) \
MilkTea_log(DEBUG, TAG, ##__VA_ARGS__)

#define MilkTea_logI(...) \
MilkTea_log(INFO, TAG, ##__VA_ARGS__)

#define MilkTea_logW(...) \
MilkTea_log(WARN, TAG, ##__VA_ARGS__)

#define MilkTea_logE(...) \
MilkTea_log(ERROR, TAG, ##__VA_ARGS__)

#endif // ifndef LIB_MILKTEA_CORE_LOGGER_H_
