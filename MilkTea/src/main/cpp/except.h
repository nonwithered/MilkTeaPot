#ifndef MILKTEA_EXCEPTION_H_
#define MILKTEA_EXCEPTION_H_

#include <string>
#include <string_view>

#include <milktea/common.h>

namespace MilkTea {

class ExceptionImpl final : public std::exception {
 public:
  static std::string_view What(std::string_view = nullptr);
  ExceptionImpl(Exception::Type type, std::string what) : type_(type), what_(what) {}
  const char* what() const noexcept final {
    return what_.data();
  }
  Exception::Type type() const {
    return type_;
  }
 private:
  const Exception::Type type_;
  const std::string what_;
};

} // namespace MilkTea

#endif // ifndef MILKTEA_EXCEPTION_H_
