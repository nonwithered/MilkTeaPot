#ifndef LIB_MILKTEA_WRAPPER_LOGGER_H_
#define LIB_MILKTEA_WRAPPER_LOGGER_H_

#include <milktea/common.h>

namespace MilkTea {

namespace Logger {

template<typename T>
bool Config(T &&logger) {
  return MilkTea_Logger_Config(std::forward<T>(logger).ToRawType());
}

inline
Level GetLevel() {
  return FromRawType(MilkTea_Logger_GetLevel());
}

inline
void Print(Level level, std::string_view tag, std::string_view msg) {
  MilkTea_Logger_Print(ToRawType(level), tag.data(), msg.data());
}

} // namespace Logger

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_WRAPPER_LOGGER_H_
