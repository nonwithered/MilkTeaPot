#ifndef LIB_TEA_DEF_H_
#define LIB_TEA_DEF_H_

#ifdef __cplusplus
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <cctype>
#else // ifdef __cplusplus
#include <stdnoreturn.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <ctype.h>
#endif // ifdef __cplusplus

#ifndef TEA_API
  #ifdef __GNUC__
    #define TEA_API __attribute__((visibility("default")))
  #endif // ifdef __GNUC__
#endif // ifndef TEA_API

#ifndef TEA_API
  #ifdef _MSC_VER
    #define TEA_API __declspec(dllexport)
  #endif // ifdef _MSC_VER
#endif // ifndef TEA_API

#ifndef TEA_API
  #define TEA_API
#endif // ifndef TEA_API

#ifndef TEA_CALL
  #if defined(__GNUC__) && defined(__i386__)
    #define TEA_CALL __attribute__((stdcall))
  #endif // if defined(__GNUC__) && defined(__i386__)
#endif // ifndef TEA_CALL

#ifndef TEA_CALL
  #ifdef _MSC_VER
    #define TEA_CALL __stdcall
  #endif // ifdef _MSC_VER
#endif // ifndef TEA_CALL

#ifndef TEA_CALL
  #define TEA_CALL
#endif // ifndef TEA_CALL

#ifndef TEA_WEAK
  #ifdef __cplusplus
    #define TEA_WEAK inline
  #endif // ifdef __cplusplus
#endif // ifndef TEA_WEAK

#ifndef TEA_WEAK
  #if defined(__GNUC__)
    #define TEA_WEAK __attribute__((weak))
  #endif // if defined(__GNUC__)
#endif // ifndef TEA_WEAK

#ifndef TEA_WEAK
  #define TEA_WEAK static
#endif // ifndef TEA_WEAK

#ifndef TEA_NORETURN
  #ifdef __cplusplus
    #define TEA_NORETURN [[noreturn]]
  #else // ifdef __cplusplus
    #define TEA_NORETURN noreturn
  #endif // ifdef __cplusplus
#endif // ifndef TEA_NORETURN

#ifndef TEA_NULL
  #ifdef __cplusplus
    #define TEA_NULL nullptr
  #else // ifdef __cplusplus
    #define TEA_NULL NULL
  #endif // ifdef __cplusplus
#endif // ifndef TEA_NULL

#endif // ifndef LIB_TEA_DEF_H_
