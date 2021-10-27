#include <milktea.h>

namespace {

constexpr char TAG[] = "MilkTea#extern";

void LogDefault(const char *, const char *) {}

void MilkTea_CALL MilkTea_BaseLogger_debug(void *obj, const char *tag, const char *msg) {
  auto &logger = *reinterpret_cast<MilkTea::BaseLogger *>(obj);
  logger.Debug(tag, msg);
}

void MilkTea_CALL MilkTea_BaseLogger_info(void *obj, const char *tag, const char *msg) {
  auto &logger = *reinterpret_cast<MilkTea::BaseLogger *>(obj);
  logger.Info(tag, msg);
}

void MilkTea_CALL MilkTea_BaseLogger_warn(void *obj, const char *tag, const char *msg) {
  auto &logger = *reinterpret_cast<MilkTea::BaseLogger *>(obj);
  logger.Warn(tag, msg);
}

void MilkTea_CALL MilkTea_BaseLogger_error(void *obj, const char *tag, const char *msg) {
  auto &logger = *reinterpret_cast<MilkTea::BaseLogger *>(obj);
  logger.Error(tag, msg);
}

} // namespace

extern "C" {

const char *MilkTea_Exception_What() {
  return MilkTea::Exception::WhatMessage();
}

void MilkTea_Logger_Init(MilkTea_Logger_t logger) {
  MilkTea::Logger logger_(
    MilkTea::Logger::LevelOf(logger.level),
      [logger](const char *tag, const char *msg) -> void {
        logger.debug(logger.obj, tag, msg);
      },
      [logger](const char *tag, const char *msg) -> void {
        logger.info(logger.obj, tag, msg);
      },
      [logger](const char *tag, const char *msg) -> void {
        logger.warn(logger.obj, tag, msg);
      },
      [logger](const char *tag, const char *msg) -> void {
        logger.error(logger.obj, tag, msg);
      }
  );
  MilkTea::Logger::Instance(&logger_);
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
  static Logger logger_(Logger::Level::ASSERT, LogDefault, LogDefault, LogDefault, LogDefault);
  if (logger != nullptr) {
    logger_ = *logger;
  }
  return logger_;
}

MilkTea_Logger_t BaseLogger::RawLogger() {
  return MilkTea_Logger_t{
    .obj = reinterpret_cast<void *>(this),
    .level = RawLevel(level()),
    .debug = MilkTea_BaseLogger_debug,
    .info = MilkTea_BaseLogger_info,
    .warn = MilkTea_BaseLogger_warn,
    .error = MilkTea_BaseLogger_error,
  };
}

} // namespace MilkTea
