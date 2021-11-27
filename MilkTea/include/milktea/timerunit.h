#ifndef LIB_MILKTEA_TIMERUNIT_H_
#define LIB_MILKTEA_TIMERUNIT_H_

#ifdef __cplusplus
#include <milktea/timer.h>
#include <chrono>
namespace MilkTea {
namespace TimerUnit {
using clock_type = std::chrono::system_clock;
using duration_type = std::chrono::milliseconds;
using time_point_type = std::chrono::time_point<clock_type, duration_type>;
} // namespace TimerUnit
} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_TIMERUNIT_H_
