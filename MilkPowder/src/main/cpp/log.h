#ifndef MILKPOWDER_LOG_H_
#define MILKPOWDER_LOG_H_

#include <milktea.h>

#define LOG_BUFFER_SIZE 128

#ifdef NDEBUG
#define LOG_PRINT(L, TAG, ...) \
do { \
} while (false)
#else // ifdef NDEBUG
#define LOG_PRINT(L, TAG, ...) \
do { \
  MilkTea::Logger logger = MilkPowder::Log(); \
  if (logger.level() <= MilkTea::Logger::Level::L) { \
    char buf[LOG_BUFFER_SIZE]; \
    snprintf(buf, LOG_BUFFER_SIZE, ##__VA_ARGS__); \
    logger(MilkTea::Logger::Level::L, TAG, buf); \
  } \
} while (false)
#endif // ifdef NDEBUG

namespace MilkPowder {

MilkTea::Logger &Log(MilkTea::Logger *logger = nullptr);

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_LOG_H_
