#ifndef LIB_MILKTEA_UTIL_H_
#define LIB_MILKTEA_UTIL_H_

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkTea_WIN
#endif

#ifndef MilkTea_API
  #ifdef MilkTea_WIN
    #ifdef MilkTea_dynlink
      #define MilkTea_API __declspec(dllimport)
    #else // ifdef MilkTea_dynlink
      #define MilkTea_API __declspec(dllexport)
    #endif // ifdef MilkTea_dynlink
  #endif // ifdef MilkTea_WIN
#endif // ifndef MilkTea_API

#ifndef MilkTea_API
  #ifdef __GNUC__
    #define MilkTea_API __attribute__((visibility("default")))
  #endif // ifdef __GNUC__
#endif // ifndef MilkTea_API

#ifndef MilkTea_API
#define MilkTea_API
#endif

#ifndef MilkTea_CALL
  #ifdef MilkTea_WIN
    #define MilkTea_CALL __stdcall
  #endif // ifdef MilkTea_WIN
#endif // ifndef MilkTea_CALL

#ifndef MilkTea_CALL
  #ifdef __GNUC__
    #ifdef __i386__
      #define MilkTea_CALL __attribute__((__stdcall__))
    #endif // ifdef __i386__
  #endif // ifdef __GNUC__
#endif // ifndef MilkTea_CALL

#ifndef MilkTea_CALL
#define MilkTea_CALL
#endif

#ifdef MilkTea_WIN
#undef MilkTea_WIN
#endif // ifdef MilkTea_WIN

#ifdef __cplusplus

#define MilkTea_IMPL(section, list, block) \
MilkTea_Exception_t \
section list { \
  MilkTea_logD("begin " #section); \
  MilkTea::Defer defer_([]() -> void { MilkTea_logD("end " #section); }); \
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
