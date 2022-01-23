#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <milktea.h>
#include <soybean.h>

namespace Milk {

static constexpr char kName[] = "milk";

static constexpr char kVersion[] = "0.0.1-SNAPSHOT";

using args_type = typename std::vector<std::string_view>;

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
