#ifndef LIB_MILKTEA_COMMON_LOGGER_H_
#define LIB_MILKTEA_COMMON_LOGGER_H_

#include <functional>
#include <memory>

#include <milktea/core.h>

namespace MilkTea {

namespace Logger {

enum class Level {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT
};

namespace Mapping {

template<Level T>
struct ToRawType;
template<>
struct ToRawType<Level::DEBUG> {
  static constexpr auto target = MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG;
};
template<>
struct ToRawType<Level::INFO> {
  static constexpr auto target = MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO;
};
template<>
struct ToRawType<Level::WARN> {
  static constexpr auto target = MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN;
};
template<>
struct ToRawType<Level::ERROR> {
  static constexpr auto target = MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR;
};
template<>
struct ToRawType<Level::ASSERT> {
  static constexpr auto target = MilkTea_Logger_Level_t::MilkTea_Logger_Level_ASSERT;
};

template<MilkTea_Logger_Level_t T>
struct FromRawType;
template<>
struct FromRawType<MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG> {
  static constexpr auto target = Level::DEBUG;
};
template<>
struct FromRawType<MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO> {
  static constexpr auto target = Level::INFO;
};
template<>
struct FromRawType<MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN> {
  static constexpr auto target = Level::WARN;
};
template<>
struct FromRawType<MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR> {
  static constexpr auto target = Level::ERROR;
};
template<>
struct FromRawType<MilkTea_Logger_Level_t::MilkTea_Logger_Level_ASSERT> {
  static constexpr auto target = Level::ASSERT;
};

} // namespace Mapping

inline MilkTea_Logger_Level_t ToRawType(Level level) {
  switch (level) {
  case Level::DEBUG: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG;
  case Level::INFO: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO;
  case Level::WARN: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN;
  case Level::ERROR: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR;
  default: return MilkTea_Logger_Level_t::MilkTea_Logger_Level_ASSERT;
  }
}

inline Level FromRawType(MilkTea_Logger_Level_t level) {
  switch (level) {
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_DEBUG: return Level::DEBUG;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_INFO: return Level::INFO;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_WARN: return Level::WARN;
  case MilkTea_Logger_Level_t::MilkTea_Logger_Level_ERROR: return Level::ERROR;
  default: return Level::ASSERT;
  }
}

} // namespace Logger
} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_LOGGER_H_
