#ifndef LIB_MILKTEA_COMMON_EXCEPT_H_
#define LIB_MILKTEA_COMMON_EXCEPT_H_

#include <string_view>
#include <exception>
#include <tuple>

#include <milktea/common/macro.h>

namespace MilkTea {

namespace Exception {

enum class Type {
  Nil,
  Unknown,
  Assertion,
  NullPointer,
  Unsupported,
  InvalidParam,
  LogicError,
  IOError,
};

inline
MilkTea_Exception_t ToRawType(Type type) {
  switch (type) {
    case Type::Nil: return MilkTea_Exception_t::MilkTea_Exception_Nil;
    case Type::Unknown: return MilkTea_Exception_t::MilkTea_Exception_Unknown;
    case Type::Assertion: return MilkTea_Exception_t::MilkTea_Exception_Assertion;
    case Type::NullPointer: return MilkTea_Exception_t::MilkTea_Exception_NullPointer;
    case Type::Unsupported: return MilkTea_Exception_t::MilkTea_Exception_Unsupported;
    case Type::InvalidParam: return MilkTea_Exception_t::MilkTea_Exception_InvalidParam;
    case Type::LogicError: return MilkTea_Exception_t::MilkTea_Exception_LogicError;
    case Type::IOError: return MilkTea_Exception_t::MilkTea_Exception_IOError;
    default: return MilkTea_Exception_t::MilkTea_Exception_Nil;
  }
}
inline
Type FromRawType(MilkTea_Exception_t type) {
  switch (type) {
    case MilkTea_Exception_t::MilkTea_Exception_Nil: return Type::Nil;
    case MilkTea_Exception_t::MilkTea_Exception_Unknown: return Type::Unknown;
    case MilkTea_Exception_t::MilkTea_Exception_Assertion: return Type::Assertion;
    case MilkTea_Exception_t::MilkTea_Exception_NullPointer: return Type::NullPointer;
    case MilkTea_Exception_t::MilkTea_Exception_Unsupported: return Type::Unsupported;
    case MilkTea_Exception_t::MilkTea_Exception_InvalidParam: return Type::InvalidParam;
    case MilkTea_Exception_t::MilkTea_Exception_LogicError: return Type::LogicError;
    case MilkTea_Exception_t::MilkTea_Exception_IOError: return Type::IOError;
    default: return Type::Nil;
  }
}

inline
std::string_view TypeName(Type type) {
  switch (type) {
    case Type::Nil: return "Nil";
    case Type::Unknown: return "Unknown";
    case Type::Assertion: return "Assertion";
    case Type::NullPointer: return "NullPointer";
    case Type::Unsupported: return "Unsupported";
    case Type::InvalidParam: return "InvalidParam";
    case Type::LogicError: return "LogicError";
    case Type::IOError: return "IOError";
    default: return "";
  }
}

} // namespace Exception
} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_EXCEPT_H_
