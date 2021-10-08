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

#define MilkTea_Exception_What(P) \
P##_Exception_What

#define MilkTea_Exception_What_Decl(P) \
MilkTea_API const char * \
MilkTea_Exception_What(P)()

#ifdef __cplusplus
#include <milktea/util.h>
#include <exception>
#include <string>
#include <cstdio>
namespace MilkTea {

#define MilkTea_Exception_What_Impl(P) \
namespace { \
const char *MilkTea_##P##_Exception_What(const char *what = nullptr) { \
  thread_local static std::string what_; \
  if (what != nullptr) { \
    what_ = what; \
  } \
  return what_.data(); \
} \
} /* namespace */ \
extern "C" { \
MilkTea_Exception_What_Decl(P)() { \
  return MilkTea_##P##_Exception_What(); \
} \
} /* extern "C" */

constexpr size_t kWhatMaxSize = 128;

#define MILKTEA_EXCEPTION_THROW(type, what) \
do { \
  throw MilkTea::Exception(MilkTea::Exception::Type::type, what); \
} while (false)

#define MILKTEA_EXCEPTION_THROW_FORMAT(type, ...) \
do { \
  char what[MilkTea::kWhatMaxSize]; \
  snprintf(what, MilkTea::kWhatMaxSize, ##__VA_ARGS__); \
  THROW(type, what); \
} while (false)

#define MILKTEA_EXCEPTION_THROW_NULL(name) \
do { \
  if (name == nullptr) { \
    MILKTEA_EXCEPTION_THROW(NullPointer, #name); \
  } \
} while (false)

#define MILKTEA_EXCEPTION_WITH(P, block) \
do { \
  try { \
    block \
  } catch (MilkTea::Except &e) { \
    MilkTea_Exception_What(P)(e.what()); \
    return e.Raw(); \
  } catch (std::exception &e) { \
    MilkTea_Exception_What(P)(e.what()); \
    return MilkTea_Exception_t::MilkTea_Exception_Unknown; \
  } \
  MilkTea_Exception_What(P)(""); \
  return MilkTea_Exception_t::MilkTea_Exception_Nil; \
} while (false)

#define MilkTea_API_Impl(P, section, list, block) \
MilkTea_API MilkTea_Exception_t \
section list { \
  LOG_PRINT(DEBUG, "api_begin", #section); \
  MilkTea::Defer defer([]() -> void { LOG_PRINT(DEBUG, "api_end", #section); }); \
  MILKTEA_EXCEPTION_WITH(P, block); \
}

class Exception final : public std::exception {
 public:
  enum class Type {
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
  MilkTea_Exception_t Raw() const {
    switch (type_) {
        case Type::Assertion: return MilkTea_Exception_t::MilkTea_Exception_Assertion;
        case Type::NullPointer: return MilkTea_Exception_t::MilkTea_Exception_NullPointer;
        case Type::Unsupported: return MilkTea_Exception_t::MilkTea_Exception_Unsupported;
        case Type::EndOfFile: return MilkTea_Exception_t::MilkTea_Exception_EndOfFile;
        case Type::InvalidParam: return MilkTea_Exception_t::MilkTea_Exception_InvalidParam;
        case Type::LogicError: return MilkTea_Exception_t::MilkTea_Exception_LogicError;
        default: return MilkTea_Exception_t::MilkTea_Exception_Unknown;
    }
  }
 private:
  const Type type_;
  const std::string what_;
};

} // namespace MilkTea
#endif // ifdef __cplusplus

#endif // ifndef LIB_MILKTEA_EXCEPTION_H_
