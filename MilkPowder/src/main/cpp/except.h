#ifndef MILKPOWDER_EXCEPT_H_
#define MILKPOWDER_EXCEPT_H_

#include <cstdint>
#include <exception>
#include <string>

#define ERR_BUFFER_SIZE 128

#define THROW(type, what) \
do { \
  throw MilkPowder::Except(MilkPowder::Except::Type::type, what); \
} while (false)

#define THROW_FORMAT(type, ...) \
do { \
  char buf[ERR_BUFFER_SIZE]; \
  snprintf(buf, ERR_BUFFER_SIZE, ##__VA_ARGS__); \
  THROW(type, buf); \
} while (false)

#define THROW_IF_NULL(name) \
do { \
  if (name == nullptr) { \
    THROW(NullPointer, #name); \
  } \
} while (false)

namespace MilkPowder {

class Except final : public std::exception {
 public:
  enum class Type : uint8_t {
    Assertion,
    NullPointer,
    Unsupported,
    EndOfFile,
    InvalidParam,
    LogicError
  };
  Except(Type type, std::string what) : type_(type), what_(what) {}
  const char* what() const noexcept final {
    return what_.data();
  }
  Type type() const {
    return type_;
  }
 private:
  const Type type_;
  const std::string what_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_EXCEPT_H_
