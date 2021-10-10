#ifndef SOYBEAN_LOG_H_
#define SOYBEAN_LOG_H_

#include <milktea.h>

#define LOG_PRINT(L, TAG, ...) \
MilkTea_Logger_LogPrint(SoyBean, L, TAG, ##__VA_ARGS__)

MilkTea_Logger_Log_DECL(SoyBean)

#endif // ifndef SOYBEAN_LOG_H_
