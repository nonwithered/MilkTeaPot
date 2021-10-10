#ifndef LIB_MILKTEA_EXCEPTION_H_
#define LIB_MILKTEA_EXCEPTION_H_

#include <milktea/util.h>

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

#ifdef __cplusplus
#include <milktea/util.h>
#include <exception>
#include <string>
#include <cstdio>
namespace MilkTea {

#define MilkTea_Exception_What_DECL(P) \
namespace P { \
const char *MilkTea_Exception_What(const char *what = nullptr); \
} /* namespace */

#define MilkTea_Exception_What_IMPL(P) \
const char *P::MilkTea_Exception_What(const char *what) { \
  thread_local static std::string what_; \
  if (what != nullptr) { \
    what_ = what; \
  } \
  return what_.data(); \
}

constexpr size_t kWhatMaxSize = 128;

#define MILKTEA_EXCEPTION_THROW(type, what) \
do { \
  throw MilkTea::Exception(MilkTea::Exception::Type::type, what); \
} while (false)

#define MILKTEA_EXCEPTION_THROW_FORMAT(type, ...) \
do { \
  char what[MilkTea::kWhatMaxSize]; \
  snprintf(what, MilkTea::kWhatMaxSize, ##__VA_ARGS__); \
  MILKTEA_EXCEPTION_THROW(type, what); \
} while (false)

#define MILKTEA_EXCEPTION_THROW_NULL(name) \
do { \
  if (name == nullptr) { \
    MILKTEA_EXCEPTION_THROW(NullPointer, #name); \
  } \
} while (false)

#define MILKTEA_EXCEPTION_WITH(P, block) { \
  try { \
    block \
  } catch (MilkTea::Exception &e) { \
    P::MilkTea_Exception_What(e.what()); \
    return e.GetRawType(); \
  } catch (std::exception &e) { \
    P::MilkTea_Exception_What(e.what()); \
    return MilkTea_Exception_t::MilkTea_Exception_Unknown; \
  } \
  P::MilkTea_Exception_What(""); \
  return MilkTea_Exception_t::MilkTea_Exception_Nil; \
}

class Exception final : public std::exception {
 public:
  enum class Type {
    Unknown,
    Assertion,
    NullPointer,
    Unsupported,
    InvalidParam,
    LogicError,
    EndOfFile
  };
  Exception(Type type, std::string what) : Exception(type) {}
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
  static void ThrowFromRawType(MilkTea_Exception_t type) {
    switch (type) {
      case MilkTea_Exception_t::MilkTea_Exception_Nil: return;
      case MilkTea_Exception_t::MilkTea_Exception_Assertion: throw Exception(Type::Assertion);
      case MilkTea_Exception_t::MilkTea_Exception_NullPointer: throw Exception(Type::NullPointer);
      case MilkTea_Exception_t::MilkTea_Exception_Unsupported: throw Exception(Type::Unsupported);
      case MilkTea_Exception_t::MilkTea_Exception_InvalidParam: throw Exception(Type::InvalidParam);
      case MilkTea_Exception_t::MilkTea_Exception_LogicError: throw Exception(Type::LogicError);
      case MilkTea_Exception_t::MilkTea_Exception_EndOfFile: throw Exception(Type::EndOfFile);
      default: throw Exception(Type::Unknown);
    }
  }
 private:
  Exception(Type type) : type_(type), what_("") {}
  const Type type_;
  const std::string what_;
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_EXCEPTION_H_
