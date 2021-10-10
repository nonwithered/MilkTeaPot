#ifndef MILKPOWDER_LOG_H_
#define MILKPOWDER_LOG_H_

#include <milktea.h>

#define LOG_PRINT(L, TAG, ...) \
MilkTea_Logger_LogPrint(L, TAG, ##__VA_ARGS__)

#endif // ifndef MILKPOWDER_LOG_H_
