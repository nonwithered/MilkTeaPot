#ifndef SOYBEAN_WINDOWS_LOG_H_
#define SOYBEAN_WINDOWS_LOG_H_

#include <milktea.h>

MilkTea_Logger_Log_DECL(SoyBean_Windows)

#define LOG_PRINT(L, TAG, ...) \
MilkTea_Logger_LogPrint(SoyBean_Windows, L, TAG, ##__VA_ARGS__)

#endif // ifndef SOYBEAN_WINDOWS_LOG_H_
