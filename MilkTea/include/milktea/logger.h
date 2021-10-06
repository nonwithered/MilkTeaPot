#ifndef LIB_MILKTEA_LOGGER_H_
#define LIB_MILKTEA_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_LogLevel_t {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT
};

struct MilkTea_Logger_t {
  void *obj;
  void (*debug)(void *obj, const char *tag, const char *msg);
  void (*info)(void *obj, const char *tag, const char *msg);
  void (*warn)(void *obj, const char *tag, const char *msg);
  void (*error)(void *obj, const char *tag, const char *msg);
  enum MilkTea_LogLevel_t level;
};

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
typedef enum MilkTea_LogLevel_t MilkTea_LogLevel_t;
typedef enum MilkTea_Logger_t MilkTea_Logger_t;
#endif // ifndef __cplusplus


#endif // ifndef LIB_MILKTEA_LOGGER_H_
