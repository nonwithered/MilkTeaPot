#ifndef SOYBEAN_WINDOWS_LOG_H_
#define SOYBEAN_WINDOWS_LOG_H_

#include <milktea.h>

#define LOG_PRINT(L, TAG, ...) \
MilkTea_Logger_LogPrint(L, TAG, ##__VA_ARGS__)

#endif // ifndef SOYBEAN_WINDOWS_LOG_H_
