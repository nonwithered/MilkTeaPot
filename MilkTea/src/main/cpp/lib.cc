#include <milktea.h>

extern "C" {

MilkTea_API void
MilkTea_Log_Init(MilkTea_Logger_t log) {
  MilkTea::Logger log_(log);
  MilkTea::Logger::Instance(&log_);
}

} // extern "C"

namespace MilkTea {

Logger &Logger::Instance(Logger *logger) {
  static Logger logger_;
  if (logger != nullptr) {
    logger_ = *logger;
  }
  return logger_;
}

} // namespace MilkTea
