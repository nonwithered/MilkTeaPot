#ifndef LIB_MILKTEA_UTIL_H_
#define LIB_MILKTEA_UTIL_H_

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkTea_API __declspec(dllexport)
#else
#define MilkTea_API __attribute__((visibility("default")))
#endif

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkTea_CALL __stdcall
#else
#define MilkTea_CALL __attribute__((__stdcall__))
#endif

#ifdef __cplusplus

#define MilkTea_IMPL(section, list, block) \
MilkTea_Exception_t \
section list { \
  MilkTea_LogPrint(DEBUG, TAG, "begin " #section); \
  MilkTea::Defer defer_([]() -> void { MilkTea_LogPrint(DEBUG, TAG, "end " #section); }); \
  MilkTea_with_except(block) \
}

#define MilkTea_NonCopy(T) \
  T(const T &) = delete; \
  void operator=(const T &) = delete;
#define MilkTea_NonMove(T) \
  T(T &&) = delete; \
  void operator=(T &&) = delete;
#define MilkTea_NonAssign(T) \
  void operator=(const T &) = delete; \
  void operator=(T &&) = delete;

#endif // ifdef __cplusplus

#ifdef __cplusplus
#include <cstdint>
#include <cinttypes>
#include <cctype>
#else // ifdef __cplusplus
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_UTIL_H_
