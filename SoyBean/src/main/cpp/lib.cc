#include <soybean.h>

#include "except.h"

namespace {

constexpr char TAG[] = "api";

const char *ExceptWhat(const char *what = nullptr) {
  thread_local static std::string what_;
  if (what != nullptr) {
    what_ = what;
  }
  return what_.data();
}

SoyBean_Err_t soymilk_errno_map(SoyBean::Except::Type type) {
  switch (type) {
    case SoyBean::Except::Type::Assertion: return SoyBean_Err_t::SoyBean_Err_Assertion;
    case SoyBean::Except::Type::NullPointer: return SoyBean_Err_t::SoyBean_Err_NullPointer;
    case SoyBean::Except::Type::Unsupported: return SoyBean_Err_t::SoyBean_Err_Unsupported;
    case SoyBean::Except::Type::InvalidParam: return SoyBean_Err_t::SoyBean_Err_InvalidParam;
    case SoyBean::Except::Type::LogicError: return SoyBean_Err_t::SoyBean_Err_LogicError;
    default: return SoyBean_Err_t::SoyBean_Err_Unknown;
  }
}

} // namespace

#define with_except(block) \
do { \
  try { \
    block \
  } catch (SoyBean::Except &e) { \
    ExceptWhat(e.what()); \
    return soymilk_errno_map(e.type()); \
  } catch (std::exception &e) { \
    ExceptWhat(e.what()); \
    return SoyBean_Err_t::SoyBean_Err_Unknown; \
  } \
  ExceptWhat(""); \
  return SoyBean_Err_t::SoyBean_Err_Nil; \
} while (false)

#define API_IMPL(section, list, block) \
SoyBean_API SoyBean_Err_t \
section list { \
  LOG_PRINT(DEBUG, "api_begin", #section); \
  MilkTea::Defer defer([]() -> void { LOG_PRINT(DEBUG, "api_end", #section); }); \
  with_except(block); \
}
