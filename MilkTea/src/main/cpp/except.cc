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

Exception::Type Exception::Unwrap(std::exception *e) {
  if (e == nullptr) {
    ExceptionImpl::What("");
    return Exception::Type::Nil;
  }
  ExceptionImpl *e_ = dynamic_cast<ExceptionImpl *>(e);
  if (e_ == nullptr) {
    ExceptionImpl::What(e->what());
    return Exception::Type::Unknown;
  }
  ExceptionImpl::What(e_->what());
  return e_->type();
}

} // namespace MilkTea
