#include <milktea.h>

#include "except.h"

namespace {

constexpr char TAG[] = "MilkTea";

} // namespace

namespace MilkTea {

std::string_view ExceptionImpl::What(std::string_view what) {
  thread_local static std::string what_;
  if (what != nullptr) {
    what_ = what;
  }
  return what_;
}

} // namespace MilkTea
