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
  EndOfFile,
};

namespace Mapping {

template<Type T>
struct ToRawType;
template<>
struct ToRawType<Type::Nil> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_Nil;
};
template<>
struct ToRawType<Type::Unknown> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_Unknown;
};
template<>
struct ToRawType<Type::Assertion> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_Assertion;
};
template<>
struct ToRawType<Type::NullPointer> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_NullPointer;
};
template<>
struct ToRawType<Type::Unsupported> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_Unsupported;
};
template<>
struct ToRawType<Type::InvalidParam> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_InvalidParam;
};
template<>
struct ToRawType<Type::LogicError> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_LogicError;
};
template<>
struct ToRawType<Type::EndOfFile> {
  static constexpr auto target = MilkTea_Exception_t::MilkTea_Exception_EndOfFile;
};

template<MilkTea_Exception_t T>
struct FromRawType;
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_Nil> {
  static constexpr auto target = Type::Nil;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_Unknown> {
  static constexpr auto target = Type::Unknown;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_Assertion> {
  static constexpr auto target = Type::Assertion;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_NullPointer> {
  static constexpr auto target = Type::NullPointer;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_Unsupported> {
  static constexpr auto target = Type::Unsupported;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_InvalidParam> {
  static constexpr auto target = Type::InvalidParam;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_LogicError> {
  static constexpr auto target = Type::LogicError;
};
template<>
struct FromRawType<MilkTea_Exception_t::MilkTea_Exception_EndOfFile> {
  static constexpr auto target = Type::EndOfFile;
};

} // namespace Mapping

inline MilkTea_Exception_t ToRawType(Type type) {
  switch (type) {
    case Type::Nil: return MilkTea_Exception_t::MilkTea_Exception_Nil;
    case Type::Unknown: return MilkTea_Exception_t::MilkTea_Exception_Unknown;
    case Type::Assertion: return MilkTea_Exception_t::MilkTea_Exception_Assertion;
    case Type::NullPointer: return MilkTea_Exception_t::MilkTea_Exception_NullPointer;
    case Type::Unsupported: return MilkTea_Exception_t::MilkTea_Exception_Unsupported;
    case Type::InvalidParam: return MilkTea_Exception_t::MilkTea_Exception_InvalidParam;
    case Type::LogicError: return MilkTea_Exception_t::MilkTea_Exception_LogicError;
    case Type::EndOfFile: return MilkTea_Exception_t::MilkTea_Exception_EndOfFile;
    default: return MilkTea_Exception_t::MilkTea_Exception_Nil;
  }
}
inline Type FromRawType(MilkTea_Exception_t type) {
  switch (type) {
    case MilkTea_Exception_t::MilkTea_Exception_Nil: return Type::Nil;
    case MilkTea_Exception_t::MilkTea_Exception_Unknown: return Type::Unknown;
    case MilkTea_Exception_t::MilkTea_Exception_Assertion: return Type::Assertion;
    case MilkTea_Exception_t::MilkTea_Exception_NullPointer: return Type::NullPointer;
    case MilkTea_Exception_t::MilkTea_Exception_Unsupported: return Type::Unsupported;
    case MilkTea_Exception_t::MilkTea_Exception_InvalidParam: return Type::InvalidParam;
    case MilkTea_Exception_t::MilkTea_Exception_LogicError: return Type::LogicError;
    case MilkTea_Exception_t::MilkTea_Exception_EndOfFile: return Type::EndOfFile;
    default: return Type::Nil;
  }
}

} // namespace Exception
} // namespace MilkTea

#endif // ifndef LIB_MILKTEA_COMMON_EXCEPT_H_
