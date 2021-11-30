#ifndef LIB_MILKTEA_UTIL_H_
#define LIB_MILKTEA_UTIL_H_

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkTea_WIN
#endif

#ifndef MilkTea_api
  #ifdef MilkTea_WIN
    #ifdef MilkTea_dynlink
      #define MilkTea_api __declspec(dllimport)
    #else // ifdef MilkTea_dynlink
      #define MilkTea_api __declspec(dllexport)
    #endif // ifdef MilkTea_dynlink
  #endif // ifdef MilkTea_WIN
#endif // ifndef MilkTea_api

#ifndef MilkTea_api
  #ifdef __GNUC__
    #define MilkTea_api __attribute__((visibility("default")))
  #endif // ifdef __GNUC__
#endif // ifndef MilkTea_api

#ifndef MilkTea_api
#define MilkTea_api
#endif

#ifndef MilkTea_call
  #ifdef MilkTea_WIN
    #define MilkTea_call __stdcall
  #endif // ifdef MilkTea_WIN
#endif // ifndef MilkTea_call

#ifndef MilkTea_call
  #ifdef __GNUC__
    #ifdef __i386__
      #define MilkTea_call __attribute__((__stdcall__))
    #endif // ifdef __i386__
  #endif // ifdef __GNUC__
#endif // ifndef MilkTea_call

#ifndef MilkTea_call
#define MilkTea_call
#endif

#ifdef MilkTea_WIN
#undef MilkTea_WIN
#endif // ifdef MilkTea_WIN

#define MilkTea_block(b) do b while (false)

#ifdef __cplusplus

#define MilkTea_extern(section, list, block) \
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
#define MilkTea_NonCopyAssign(T) \
  void operator=(const T &) = delete;
#define MilkTea_NonMoveAssign(T) \
  void operator=(T &&) = delete;
#define MilkTea_NonAssign(T) \
  MilkTea_NonCopyAssign(T) \
  MilkTea_NonMoveAssign(T)

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
#include <stdnoreturn.h>
#endif // ifdef __cplusplus

#ifdef __cplusplus
#define MilkTea_NORETURN [[noreturn]]
#else // ifdef __cplusplus
#define MilkTea_NORETURN noreturn
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_UTIL_H_
