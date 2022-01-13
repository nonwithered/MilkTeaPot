#ifndef MILK_PRINT_H_
#define MILK_PRINT_H_

#include <string>
#include <string_view>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iostream>
#include <initializer_list>

#include <milktea.h>
#include <soybean.h>

namespace Milk {

struct StringWrapper final {
 public:
  StringWrapper(std::string s) : s_(std::move(s)) {}
  StringWrapper(std::string_view s) : s_(s) {}
  StringWrapper(const char *s) : s_(s) {}
  operator const std::string &() const {
    return s_;
  }
 private:
  const std::string s_;
};

class Printer final {
 public:
  static constexpr std::nullptr_t endl = nullptr;
  Printer() : buf_() {}
  Printer &operator()(std::initializer_list<StringWrapper> s) {
    for (const auto &it : s) {
      buf_ += it;
    }
    return *this;
  }
  template<typename T>
  Printer &operator<<(T t) {
    std::stringstream ss;
    ss << t;
    std::string s;
    ss >> s;
    buf_ += s;
    return *this;
  }
  Printer &operator<<(std::string s) {
    buf_ += s;
    return *this;
  }
  Printer &operator<<(std::string_view s) {
    buf_ += s;
    return *this;
  }
  Printer &operator<<(const char s[]) {
    buf_ += s;
    return *this;
  }
  Printer &operator<<(std::nullptr_t) {
    std::cout << buf_ << std::endl;
    buf_.clear();
    return *this;
  }
  ~Printer() {
    if (!buf_.empty()) {
      *this << endl;
    }
  }
 private:
  std::string buf_;
};

} // namespace Milk

#endif // ifndef MILK_PRINT_H_
