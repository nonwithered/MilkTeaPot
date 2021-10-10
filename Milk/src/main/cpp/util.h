#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <string>

#include <milkpowder.h>

#define check_err(s) \
do { \
  if (err != MilkTea_Exception_t::MilkTea_Exception_Nil) { \
    std::cerr << "Failed to " s " because of " << MilkTea_Exception_Name(err) << ": " << MilkPowder_Exception_What() << std::endl; \
    exit(0); \
  } \
} while (false)

namespace Milk {

constexpr char kVersion[] = "0.0.1-snapshot";

inline std::string LogTime() {
  time_t t = time(nullptr);
  tm *p = localtime(&t);
  std::stringstream ss[2];
  std::string s[2];
  ss[0] << (p->tm_year + 1900) << "-" << (p->tm_mon + 1) << "-" << p->tm_mday;
  ss[0] >> s[0];
  ss[1] << p->tm_hour << ":" << p->tm_min << ":" << p->tm_sec;
  ss[1] >> s[1];
  return s[0] + " " + s[1];
}

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
