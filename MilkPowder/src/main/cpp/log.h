#ifndef MILKPOWDER_LOG_H_
#define MILKPOWDER_LOG_H_

#include <milktea.h>

#define LOG_PRINT(L, TAG, ...) \
MilkTea_Logger_LogPrint(MilkPowder, L, TAG, ##__VA_ARGS__)

MilkTea_Logger_Log_DECL(MilkPowder)

#endif // ifndef MILKPOWDER_LOG_H_
