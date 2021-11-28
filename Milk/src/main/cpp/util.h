#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iostream>

#include <milktea.h>
#include <soybean.h>

#define check_err(s) MilkTea_block({ \
  if (err != MilkTea_Exception_t::MilkTea_Exception_Nil) { \
    std::cerr << "Failed to " s " because of " << MilkTea_Exception_Name(err) << ": " << MilkTea_Exception_What() << std::endl; \
    std::terminate(); \
  } \
})

namespace Milk {

constexpr char kVersion[] = "0.0.1-snapshot";

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
