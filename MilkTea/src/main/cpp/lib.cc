#include <milktea.h>

#include "except.h"

namespace {

constexpr char TAG[] = "MilkTea";

} // namespace

MilkTea::BaseLogger *GlobalInstance(MilkTea::LoggerWrapper *instance = nullptr);

extern "C" {

bool MilkTea_Logger_Config(MilkTea_Logger_t logger) {
  auto *logger_ = new MilkTea::LoggerWrapper(logger);
  if (GlobalInstance(logger_) == nullptr) {
    delete logger_;
    return false;
  }
  return true;
}

MilkTea_Logger_Level_t MilkTea_Logger_GetLevel() {
  return MilkTea::Logger::ToRawType(GlobalInstance()->level());
}

void MilkTea_Logger_Print(MilkTea_Logger_Level_t level, const char *tag, const char *msg) {
  GlobalInstance()->Print(MilkTea::Logger::FromRawType(level), tag, msg);
}

const char *MilkTea_Exception_What() {
  return MilkTea::ExceptionImpl::What().data();
}

MilkTea_noreturn
void MilkTea_Exception_Throw(MilkTea_Exception_t type, const char *what) {
  throw MilkTea::ExceptionImpl(MilkTea::Exception::FromRawType(type), what);
}

MilkTea_Exception_t MilkTea_Exception_Catch(MilkTea_Exception_Block_t block) {
  try {
    auto self = block.self_;
    MilkTea_nonnull(self);
    auto invoke = block.invoke_;
    MilkTea_nonnull(invoke);
    invoke(self);
  } catch (const MilkTea::ExceptionImpl &e) {
    MilkTea::ExceptionImpl::What(e.what());
    return MilkTea::Exception::ToRawType(e.type());
  } catch (const std::exception &e) {
    MilkTea::ExceptionImpl::What(e.what());
    return MilkTea::Exception::ToRawType(MilkTea::Exception::Type::Unknown);
  } catch (...) {
    MilkTea::ExceptionImpl::What("");
    return MilkTea::Exception::ToRawType(MilkTea::Exception::Type::Unknown);
  }
  MilkTea::ExceptionImpl::What("");
  return MilkTea::Exception::ToRawType(MilkTea::Exception::Type::Nil);
}

} // extern "C"
