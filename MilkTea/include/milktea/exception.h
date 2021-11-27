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
    case MilkTea_Exception_Assertion: return "Assertion";
    case MilkTea_Exception_NullPointer: return "NullPointer";
    case MilkTea_Exception_Unsupported: return "Unsupported";
    case MilkTea_Exception_InvalidParam: return "InvalidParam";
    case MilkTea_Exception_LogicError: return "LogicError";
    case MilkTea_Exception_EndOfFile: return "EndOfFile";
    default: return "Unknown";
  }
}

MilkTea_API const char * MilkTea_CALL
MilkTea_Exception_What();

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include <milktea/logger.h>
#include <exception>
#include <string>
#include <cstdio>
namespace MilkTea {

#define MilkTea_throw(type, what) MilkTea_block({ \
  MilkTea_logE("%s: %s", MilkTea_Exception_Name(MilkTea_Exception_##type), what); \
  throw MilkTea::Exception(MilkTea::Exception::Type::type, what); \
})

#define MilkTea_throwf(type, ...) MilkTea_block({ \
  constexpr size_t kWhatMaxSize = 128; \
  char what[kWhatMaxSize]; \
  snprintf(what, kWhatMaxSize, ##__VA_ARGS__); \
  MilkTea_throw(type, what); \
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

#define MilkTea_with_except(block) { \
  try { \
    block \
  } catch (MilkTea::Exception &e) { \
    MilkTea::Exception::WhatMessage(e.what()); \
    return e.GetRawType(); \
  } catch (std::exception &e) { \
    MilkTea::Exception::WhatMessage(e.what()); \
    return MilkTea_Exception_t::MilkTea_Exception_Unknown; \
  } \
  MilkTea::Exception::WhatMessage(""); \
  return MilkTea_Exception_t::MilkTea_Exception_Nil; \
}

#define MilkTea_panic(e) \
MilkTea::Exception::ThrowFromRawType(e)

class Exception final : public std::exception {
 public:
  static MilkTea_API const char * MilkTea_CALL WhatMessage(const char *what = nullptr);
  static std::tuple<MilkTea_Exception_t, const char *> ToRawType(std::exception *e) {
    if (e == nullptr) {
      return std::make_tuple(MilkTea_Exception_t::MilkTea_Exception_Nil, nullptr);
    }
    MilkTea::Exception *exception = dynamic_cast<MilkTea::Exception *>(e);
    if (exception != nullptr) {
      return std::make_tuple(exception->GetRawType(), exception->what());
    }
    return std::make_tuple(MilkTea_Exception_t::MilkTea_Exception_Unknown, e->what());
  }
  enum class Type {
    Unknown,
    Assertion,
    NullPointer,
    Unsupported,
    InvalidParam,
    LogicError,
    EndOfFile
  };
  Exception(Type type, std::string what) : type_(type), what_(what) {}
  const char* what() const noexcept final {
    return what_.data();
  }
  Type type() const {
    return type_;
  }
  MilkTea_Exception_t GetRawType() const {
    switch (type_) {
      case Type::Assertion: return MilkTea_Exception_t::MilkTea_Exception_Assertion;
      case Type::NullPointer: return MilkTea_Exception_t::MilkTea_Exception_NullPointer;
      case Type::Unsupported: return MilkTea_Exception_t::MilkTea_Exception_Unsupported;
      case Type::InvalidParam: return MilkTea_Exception_t::MilkTea_Exception_InvalidParam;
      case Type::LogicError: return MilkTea_Exception_t::MilkTea_Exception_LogicError;
      case Type::EndOfFile: return MilkTea_Exception_t::MilkTea_Exception_EndOfFile;
      default: return MilkTea_Exception_t::MilkTea_Exception_Unknown;
    }
  }
  static void ThrowFromRawType(MilkTea_Exception_t type, const char *what = nullptr) {
    if (type == MilkTea_Exception_Nil) {
      return;
    }
    throw Exception(FromRawType(type), WhatMessage(what));
  }
 private:
  static Type FromRawType(MilkTea_Exception_t type) {
    switch (type) {
      case MilkTea_Exception_t::MilkTea_Exception_Assertion: return Type::Assertion;
      case MilkTea_Exception_t::MilkTea_Exception_NullPointer: return Type::NullPointer;
      case MilkTea_Exception_t::MilkTea_Exception_Unsupported: return Type::Unsupported;
      case MilkTea_Exception_t::MilkTea_Exception_InvalidParam: return Type::InvalidParam;
      case MilkTea_Exception_t::MilkTea_Exception_LogicError: return Type::LogicError;
      case MilkTea_Exception_t::MilkTea_Exception_EndOfFile: return Type::EndOfFile;
      default: return Type::Unknown;
    }
  }
  const Type type_;
  const std::string what_;
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_EXCEPTION_H_
