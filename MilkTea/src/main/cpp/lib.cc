#include <milktea/common.h>

#include "except.h"
#include "logger.h"

namespace {

constexpr char TAG[] = "MilkTea";

MilkTea::BaseLogger *GlobalLogger(MilkTea::BaseLogger *instance = nullptr) {
  static auto instance_ = std::unique_ptr<MilkTea::BaseLogger>(nullptr);
  if (instance != nullptr) {
    instance_.reset(instance);
  }
  return instance_.get();
}

} // namespace

extern "C" {

bool MilkTea_Logger_Config(MilkTea_Logger_t logger) {
  if (GlobalLogger() != nullptr) {
    return false;
  }
  GlobalLogger(new MilkTea::LoggerWrapper(logger));
  return true;
}

MilkTea_Logger_Level_t MilkTea_Logger_GetLevel() {
  auto *logger = GlobalLogger();
  if (logger == nullptr) {
    return MilkTea::Logger::ToRawType(MilkTea::Logger::Level::ASSERT);
  }
  return MilkTea::Logger::ToRawType(logger->level());
}

void MilkTea_Logger_Print(MilkTea_Logger_Level_t level, const char *tag, const char *msg) {
  auto *logger = GlobalLogger();
  if (logger == nullptr) {
    return;
  }
  logger->Print(MilkTea::Logger::FromRawType(level), tag, msg);
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
    MilkTea_nonnull(block.self_);
    MilkTea_nonnull(block.invoke_);
    block.invoke_(block.self_);
  } catch (const MilkTea::ExceptionImpl &e) {
    MilkTea::ExceptionImpl::What(e.what());
    return MilkTea::Exception::ToRawType(e.type());
  } catch (const std::exception &e) {
    MilkTea::ExceptionImpl::What(e.what());
    return MilkTea::Exception::ToRawType(MilkTea::Exception::Type::Unknown);
  } catch (...) {
    MilkTea::ExceptionImpl::What("...");
    return MilkTea::Exception::ToRawType(MilkTea::Exception::Type::Unknown);
  }
  MilkTea::ExceptionImpl::What("");
  return MilkTea::Exception::ToRawType(MilkTea::Exception::Type::Nil);
}

} // extern "C"
