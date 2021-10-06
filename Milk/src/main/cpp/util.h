#ifndef MILK_UTIL_H_
#define MILK_UTIL_H_

#include <functional>
#include <fstream>
#include <string_view>
#include <string>
#include <cctype>
#include <cstdio>
#include <cinttypes>

#include <milkpowder.h>

#define check_err(s) \
do { \
  if (err != MilkPowder_Err_t::Nil) { \
    std::cerr << "Failed to " s " because of " << MilkPowder_Err_Name(err) << ": " << MilkPowder_Err_What() << std::endl; \
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

inline uint32_t FromBytesToU32(const uint8_t bytes[]) {
  return (bytes[0] << 030) | (bytes[1] << 020) | (bytes[2] << 010) | bytes[3];
}

inline uint16_t FromBytesToU16(const uint8_t bytes[]) {
  return (bytes[0] << 010) | bytes[1];
}

inline uint8_t FromBytesToU8(const uint8_t bytes[]) {
  return bytes[0];
}

} // namespace Milk

#endif // ifndef MILK_UTIL_H_
