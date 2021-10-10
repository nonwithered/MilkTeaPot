#ifndef LIB_MILKTEA_H_
#define LIB_MILKTEA_H_

#include <milktea/util.h>
#include <milktea/logger.h>
#include <milktea/exception.h>
#include <milktea/callback.h>
#include <milktea/tostring.h>
#include <milktea/defer.h>

#ifdef __cplusplus
extern "C" {
#endif

MilkTea_API const char *
MilkTea_Exception_What();

MilkTea_API void
MilkTea_Log_Init(MilkTea_Logger_t log);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_MILKTEA_H_
