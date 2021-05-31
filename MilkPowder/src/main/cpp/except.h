#ifndef MILKPOWDER_EXCEPT_H_
#define MILKPOWDER_EXCEPT_H_

#include <cstdint>
#include <exception>

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
  Except(Type type) : type_(type) {}
  const char* what() const noexcept final {
    switch (type_) {
			case Type::Assertion: return "Assertion";
			case Type::NullPointer: return "NullPointer";
			case Type::Unsupported: return "Unsupported";
			case Type::EndOfFile: return "EndOfFile";
			case Type::InvalidParam: return "InvalidParam";
			case Type::LogicError: return "LogicError";
			default: return "Unknown";
    }
  }
 private:
  const Type type_;
};

} // namespace MilkPowder

#endif // ifndef MILKPOWDER_EXCEPT_H_
