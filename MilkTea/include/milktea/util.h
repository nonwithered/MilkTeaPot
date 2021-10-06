#ifndef LIB_MILKTEA_UTIL_H_
#define LIB_MILKTEA_UTIL_H_

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define MilkTea_API __declspec(dllexport)
#else
#define MilkTea_API __attribute__((visibility("default")))
#endif

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
