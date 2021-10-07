#ifndef LIB_SOYMILK_CORE_H_
#define LIB_SOYMILK_CORE_H_

#include <milktea.h>

#define SoyMilk_API MilkTea_API

#ifdef __cplusplus
using SoyMilk_Logger_t = MilkTea_Logger_t;
using SoyMilk_LogLevel_t = MilkTea_LogLevel_t;
#else
typedef MilkTea_Logger_t SoyMilk_Logger_t;
typedef MilkTea_LogLevel_t SoyMilk_LogLevel_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

SoyMilk_API void
SoyMilk_Log_Init(SoyMilk_Logger_t config);

enum SoyMilk_Err_t {
  SoyMilk_Err_Nil,
  SoyMilk_Err_Unknown,
  SoyMilk_Err_Assertion,
  SoyMilk_Err_NullPointer,
  SoyMilk_Err_Unsupported,
  SoyMilk_Err_InvalidParam,
  SoyMilk_Err_LogicError
};

#ifndef __cplusplus
typedef enum SoyMilk_Err_t SoyMilk_Err_t;
#endif

SoyMilk_API const char *
SoyMilk_Err_what();

inline const char *SoyMilk_Err_Name(SoyMilk_Err_t type) {
  switch (type) {
    case SoyMilk_Err_Nil: return "Nil";
    case SoyMilk_Err_Assertion: return "Assertion";
    case SoyMilk_Err_NullPointer: return "NullPointer";
    case SoyMilk_Err_Unsupported: return "Unsupported";
    case SoyMilk_Err_InvalidParam: return "InvalidParam";
    case SoyMilk_Err_LogicError: return "LogicError";
    default: return "Unknown";
  }
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_H_
