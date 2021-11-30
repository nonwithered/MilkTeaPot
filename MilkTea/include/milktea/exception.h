#ifndef LIB_MILKTEA_EXCEPTION_H_
#define LIB_MILKTEA_EXCEPTION_H_

#include <milktea/util.h>

#ifdef __cplusplus
extern "C" {
#endif

enum MilkTea_Exception_t {
  MilkTea_Exception_Nil,
  MilkTea_Exception_Unknown,
  MilkTea_Exception_Assertion,
  MilkTea_Exception_NullPointer,
  MilkTea_Exception_Unsupported,
  MilkTea_Exception_InvalidParam,
  MilkTea_Exception_LogicError,
  MilkTea_Exception_EndOfFile
};

#ifndef __cplusplus
typedef enum MilkTea_Exception_t MilkTea_Exception_t;
#endif

inline const char *MilkTea_Exception_Name(MilkTea_Exception_t type) {
  switch (type) {
    case MilkTea_Exception_Nil: return "Nil";
    case MilkTea_Exception_Unknown: return "Unknown";
    case MilkTea_Exception_Assertion: return "Assertion";
    case MilkTea_Exception_NullPointer: return "NullPointer";
    case MilkTea_Exception_Unsupported: return "Unsupported";
    case MilkTea_Exception_InvalidParam: return "InvalidParam";
    case MilkTea_Exception_LogicError: return "LogicError";
    case MilkTea_Exception_EndOfFile: return "EndOfFile";
    default: return "";
  }
}

MilkTea_api const char * MilkTea_call
MilkTea_Exception_What();

MilkTea_NORETURN
MilkTea_api void MilkTea_call
MilkTea_Exception_Throw(MilkTea_Exception_t, const char *);

#define MilkTea_throw(type, what) MilkTea_block({ \
  MilkTea_logE("%s:%d %s: %s", __FILE__, __LINE__, MilkTea_Exception_Name(MilkTea_Exception_##type), what); \
  MilkTea_Exception_Throw(MilkTea_Exception_##type, what); \
})

#define MilkTea_throwf(type, ...) MilkTea_block({ \
  const size_t size_ = snprintf(NULL, 0, ##__VA_ARGS__) + 1; \
  char what_[size_]; \
  snprintf(what_, size_, ##__VA_ARGS__); \
  MilkTea_throw(type, what_); \
})

#define MilkTea_assert(what) \
MilkTea_throw(Assertion, what)

#define MilkTea_assertf(...) \
MilkTea_throwf(Assertion, ##__VA_ARGS__)

#define MilkTea_nonnull(sym) MilkTea_block({ \
  if (sym == nullptr) { \
    MilkTea_throw(NullPointer, #sym " is nullptr"); \
  } \
})

#define MilkTea_panic(e) MilkTea_block({ \
  if (e != MilkTea_Exception_t::MilkTea_Exception_Nil) { \
    MilkTea_Exception_Throw(e, MilkTea_Exception_What()); \
  } \
})

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

#include <exception>
#include <string_view>

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

inline MilkTea_Exception_t ToRawType(Type type) {
  switch (type) {
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

inline std::string_view What() {
  return MilkTea_Exception_What();
}

[[noreturn]]
inline void Throw(Type type, std::string_view what) {
  MilkTea_Exception_Throw(ToRawType(type), what.data());
}

MilkTea_api Type MilkTea_call
Unwrap(std::exception *e);

} // namespace Exception

#define MilkTea_with_except(block) { \
  try { \
    block \
  } catch (std::exception &e) { \
    return MilkTea::Exception::ToRawType(MilkTea::Exception::Unwrap(&e)); \
  } \
  return MilkTea::Exception::ToRawType(MilkTea::Exception::Unwrap(nullptr)); \
}

} // namespace MilkTea

#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_EXCEPTION_H_
