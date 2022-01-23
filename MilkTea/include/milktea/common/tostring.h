#ifndef LIB_MILKTEA_COMMON_TOSTRING_H_
#define LIB_MILKTEA_COMMON_TOSTRING_H_

#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cinttypes>

namespace MilkTea {

namespace ToString {

template<typename T>
std::string From(const T &t) {
  std::string s;
  std::stringstream ss;
  ss << t;
  ss >> s;
  return s;
}

inline std::string FromU8(uint8_t n) {
  char buf[4];
  sprintf(buf, "%" PRIu8, n);
  return buf;
}

inline std::string FromU16(uint16_t n) {
  char buf[8];
  sprintf(buf, "%" PRIu16, n);
  return buf;
}

inline std::string FromU32(uint32_t n) {
  char buf[16];
  sprintf(buf, "%" PRIu32, n);
  return buf;
}

inline std::string FromBytes(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    uint8_t byte = bytes[i];
    if (isgraph(byte)) {
      ss << static_cast<char>(byte);
    } else {
      ss << "\\" << std::oct << std::right << std::setw(3) << std::setfill('0') << static_cast<unsigned int>(byte);
    }
  }
  std::string s;
  ss >> s;
  return s;
}

inline std::string FromChars(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    uint8_t byte = bytes[i];
    if (isgraph(byte)) {
      ss << static_cast<char>(byte);
    } else {
      ss << ".";
    }
  }
  std::string s;
  ss >> s;
  return s;
}

} // namespace ToString

namespace ToStringHex {

inline std::string FromU8(uint8_t n) {
  char buf[3];
  sprintf(buf, "%02" PRIx8, n);
  return buf;
}

inline std::string FromU16(uint16_t n) {
  char buf[5];
  sprintf(buf, "%04" PRIx16, n);
  return buf;
}

inline std::string FromU32(uint32_t n) {
  char buf[9];
  sprintf(buf, "%08" PRIx32, n);
  return buf;
}

inline std::string FromVarLen(uint32_t n) {
  uint8_t bytes[4];
  size_t size = 0;
  bytes[size++] = static_cast<uint8_t>(n & 0x7f);
  while (n > 0x7f) {
    n >>= 07;
    bytes[size++] = static_cast<uint8_t>((n & 0x7f) | 0x80);
  }
  std::stringstream ss;
  for (size_t i = 0; i != size; ++i) {
    ss << FromU8(bytes[size - 1 -i]);
  }
  std::string s;
  ss >> s;
  return s;
}
inline std::string FromBytes(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    ss << FromU8(bytes[i]);
  }
  std::string s;
  ss >> s;
  return s;
}

} // namespace ToStringHex

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_TOSTRING_H_
