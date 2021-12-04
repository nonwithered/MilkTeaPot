#ifndef LIB_TEAPOT_COMMON_TIMERUNIT_H_
#define LIB_TEAPOT_COMMON_TIMERUNIT_H_

#include <chrono>

#include <teapot/core.h>

namespace TeaPot {

namespace TimerUnit {

constexpr char TAG[] = "TeaPot::TimerUnit";

using clock_type = std::chrono::system_clock;
using duration_type = std::chrono::milliseconds;
using time_point_type = std::chrono::time_point<clock_type, duration_type>;

} // namespace TimerUnit
} // namespace TeaPot

#endif // define LIB_TEAPOT_COMMON_TIMERUNIT_H_
