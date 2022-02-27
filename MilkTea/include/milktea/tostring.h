#ifndef LIB_MILKTEA_TOSTRING_H_
#define LIB_MILKTEA_TOSTRING_H_

#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cinttypes>
#include <initializer_list>

namespace MilkTea {

namespace ToString {

class From final {
 public:
  explicit From(std::string_view delimiter = "", std::string_view prefix = "", std::string_view suffix = "")
  : delimiter_(delimiter),
    prefix_(prefix),
    suffix_(suffix),
    ss_() {}
  template<typename ...T>
  std::string operator()(const T &...t) && {
    Append(prefix_);
    Append(t...);
    Append(suffix_);
    return ss_.str();
  }
  template<typename container_type>
  std::string operator()(typename container_type::const_iterator it, typename container_type::const_iterator n) && {
    if (it != n) {
      Append(prefix_);
      Append(*it);
      while (++it != n) {
        Append(delimiter_);
        Append(*it);
      }
      Append(suffix_);
    }
    return ss_.str();
  }
 private:
  template<typename T>
  void Append(const T &t) {
    ss_ << t;
  }
  template<typename T, typename ...U>
  void Append(const T &t, const U &...u) {
    Append(t);
    Append(delimiter_);
    Append(u...);
  }
  const std::string delimiter_;
  const std::string prefix_;
  const std::string suffix_;
  std::stringstream ss_;
};

template<std::size_t N = 256, typename ...args_type>
auto FromFormat(const char fmt[], args_type ...args) -> std::string {
  std::string str(N, '\0');
  std::snprintf(str.data(), N, fmt, std::forward<args_type>(args)...);
  return str;
}

inline
std::string FromU8(uint8_t n) {
  char buf[4];
  sprintf(buf, "%" PRIu8, n);
  return buf;
}

inline
std::string FromU16(uint16_t n) {
  char buf[8];
  sprintf(buf, "%" PRIu16, n);
  return buf;
}

inline
std::string FromU32(uint32_t n) {
  char buf[16];
  sprintf(buf, "%" PRIu32, n);
  return buf;
}

inline
std::string FromBytes(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    uint8_t byte = bytes[i];
    switch (byte) {
      case '\a':
        ss << "\\a";
        break;
      case '\b':
        ss << "\\b";
        break;
      case '\t':
        ss << "\\t";
        break;
      case '\n':
        ss << "\\n";
        break;
      case '\v':
        ss << "\\v";
        break;
      case '\f':
        ss << "\\f";
        break;
      case '\r':
        ss << "\\r";
        break;
      case '\"':
        ss << "\\\"";
        break;
      case '\'':
        ss << "\\\'";
        break;
      case '\\':
        ss << "\\\\";
        break;
      default:
        if (isgraph(byte)) {
          ss << static_cast<char>(byte);
        } else {
          ss << "\\" << std::oct << std::right << std::setw(3) << std::setfill('0') << static_cast<unsigned int>(byte);
        }
    }
  }
  return ss.str();
}

inline
std::string FromChars(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    uint8_t byte = bytes[i];
    if (isgraph(byte)) {
      ss << static_cast<char>(byte);
    } else {
      ss << ".";
    }
  }
  return ss.str();
}

} // namespace ToString

namespace ToStringHex {

inline
std::string FromU8(uint8_t n) {
  char buf[3];
  sprintf(buf, "%02" PRIx8, n);
  return buf;
}

inline
std::string FromU16(uint16_t n) {
  char buf[5];
  sprintf(buf, "%04" PRIx16, n);
  return buf;
}

inline
std::string FromU32(uint32_t n) {
  char buf[9];
  sprintf(buf, "%08" PRIx32, n);
  return buf;
}

inline
std::string FromVarLen(uint32_t n) {
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
  return ss.str();
}

inline
std::string FromBytes(const uint8_t bytes[], size_t length) {
  std::stringstream ss;
  for (size_t i = 0; i != length; ++i) {
    ss << FromU8(bytes[i]);
  }
  return ss.str();
}

} // namespace ToStringHex

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_TOSTRING_H_
