#ifndef LIB_MILKTEA_WRAPPER_FROMSTRING_H_
#define LIB_MILKTEA_WRAPPER_FROMSTRING_H_

#include <cstdint>
#include <tuple>

namespace MilkTea {

namespace FromStringDec {

template<typename T>
size_t ToInt(const char s[], size_t n, T *v = nullptr) {
  size_t i = 0;
  T r = 0;
  for (; i != n; ++i) {
    char c = s[i];
    if ('0' <= c && c <= '9') {
      uint8_t b = c - '0';
      r *= 10;
      r += b;
      continue;
    }
    break;
  }
  if (v != nullptr) {
    *v = r;
  }
  return i;
}

} // namespace FromString

namespace FromStringHex {

template<typename T>
size_t ToInt(const char s[], size_t n, T *v = nullptr) {
  size_t i = 0;
  T r = 0;
  for (; i != n; ++i) {
    char c = s[i];
    if ('0' <= c && c <= '9') {
      uint8_t b = c - '0';
      r <<= 04;
      r |= b;
      continue;
    }
    if ('a' <= c && c <= 'f') {
      uint8_t b = c - 'a' + 10;
      r <<= 04;
      r |= b;
      continue;
    }
    if ('A' <= c && c <= 'F') {
      uint8_t b = c - 'A' + 10;
      r <<= 04;
      r |= b;
      continue;
    }
    break;
  }
  if (v != nullptr) {
    *v = r;
  }
  return i;
}

} // namespace FromStringHex

namespace FromStringOct {

template<typename T>
size_t ToInt(const char s[], size_t n, T *v = nullptr) {
  size_t i = 0;
  T r = 0;
  for (; i != n; ++i) {
    char c = s[i];
    if ('0' <= c && c <= '7') {
      uint8_t b = c - '0';
      r <<= 03;
      r |= b;
      continue;
    }
    break;
  }
  if (v != nullptr) {
    *v = r;
  }
  return i;
}

} // namespace FromStringOct

namespace FromStringBin {

template<typename T>
size_t ToInt(const char s[], size_t n, T *v = nullptr) {
  size_t i = 0;
  T r = 0;
  for (; i != n; ++i) {
    char c = s[i];
    if ('0' <= c && c <= '1') {
      uint8_t b = c - '0';
      r <<= 01;
      r |= b;
      continue;
    }
    break;
  }
  if (v != nullptr) {
    *v = r;
  }
  return i;
}

} // namespace FromStringBin

} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_WRAPPER_FROMSTRING_H_
