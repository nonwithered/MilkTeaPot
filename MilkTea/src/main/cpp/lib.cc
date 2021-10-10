#include <milktea.h>

extern "C" {

MilkTea_API const char *
MilkTea_Exception_What() {
  return MilkTea::Exception::WhatMessage();
}

MilkTea_API void
MilkTea_Log_Init(MilkTea_Logger_t log) {
  MilkTea::Logger log_(log);
  MilkTea::Logger::Instance(&log_);
}

} // extern "C"

namespace MilkTea {

const char *Exception::WhatMessage(const char *what) {
  thread_local static std::string what_;
  if (what != nullptr) {
    what_ = what;
  }
  return what_.data();
}

Logger &Logger::Instance(Logger *logger) {
  static Logger logger_;
  if (logger != nullptr) {
    logger_ = *logger;
  }
  return logger_;
}

} // namespace MilkTea
