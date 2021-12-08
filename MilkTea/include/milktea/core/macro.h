#ifndef LIB_MILKTEA_CORE_MACRO_H_
#define LIB_MILKTEA_CORE_MACRO_H_

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cctype>
#else // ifdef __cplusplus
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdnoreturn.h>
#endif // ifdef __cplusplus

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkTea_WIN
#endif

#ifndef MilkTea_symbol
  #ifdef MilkTea_WIN
    #ifdef MilkTea_dynlink
      #define MilkTea_symbol __declspec(dllimport)
    #else // ifdef MilkTea_dynlink
      #define MilkTea_symbol __declspec(dllexport)
    #endif // ifdef MilkTea_dynlink
  #endif // ifdef MilkTea_WIN
#endif // ifndef MilkTea_symbol

#ifndef MilkTea_symbol
  #ifdef __GNUC__
    #define MilkTea_symbol __attribute__((visibility("default")))
  #endif // ifdef __GNUC__
#endif // ifndef MilkTea_symbol

#ifndef MilkTea_symbol
#define MilkTea_symbol
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

#ifdef __cplusplus
#define MilkTea_noreturn [[noreturn]]
#else // ifdef __cplusplus
#define MilkTea_noreturn noreturn
#endif // ifdef __cplusplus

#ifdef __cplusplus
#define MilkTea_null nullptr
#else // ifdef __cplusplus
#define MilkTea_null NULL
#endif // ifdef __cplusplus

#define MilkTea_block(b) \
do b while (false)

#define MilkTea_decl(T) \
MilkTea_symbol T MilkTea_call

#define MilkTea_api MilkTea_decl(MilkTea_Exception_t)

#endif // ifndef LIB_MILKTEA_CORE_MACRO_H_
