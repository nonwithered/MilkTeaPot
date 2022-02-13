#ifndef SOYBEAN_WINDOWS_FACTORY_H_
#define SOYBEAN_WINDOWS_FACTORY_H_

#include <soybean.h>

#include "handle.h"

namespace SoyBean_Windows {

class FactoryImpl final : public SoyBean::BaseFactory {
  static constexpr char TAG[] = "SoyBean_Windows::FactoryImpl";
 public:
  FactoryImpl(unsigned int uDeviceID, void *dwCallback, void *dwInstance, uint32_t fdwOpen)
  : enable_(true), uDeviceID_(uDeviceID), dwCallback_(dwCallback), dwInstance_(dwInstance), fdwOpen_(fdwOpen) {}
  FactoryImpl(FactoryImpl &&another) : FactoryImpl() {
    std::swap(enable_, another.enable_);
    std::swap(uDeviceID_, another.uDeviceID_);
    std::swap(dwCallback_, another.dwCallback_);
    std::swap(dwInstance_, another.dwInstance_);
    std::swap(fdwOpen_, another.fdwOpen_);
  }
  ~FactoryImpl() {
    enable_ = false;
    uDeviceID_ = 0;
    dwCallback_ = nullptr;
    dwInstance_ = nullptr;
    fdwOpen_ = 0;
  }
  BaseFactory &Move() && final {
    return *new FactoryImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  SoyBean_Handle_t Create() final {
    if (!enable_) {
      MilkTea_throw(LogicError, "Create but disable");
    }
    return HandleImpl(uDeviceID_, dwCallback_, dwInstance_, fdwOpen_).ToRawType();
  }
 private:
  FactoryImpl() : enable_(false), uDeviceID_(0), dwCallback_(nullptr), dwInstance_(nullptr), fdwOpen_(0) {}
  bool enable_;
  unsigned int uDeviceID_;
  void *dwCallback_;
  void *dwInstance_;
  uint32_t fdwOpen_;
  MilkTea_NonCopy(FactoryImpl)
  MilkTea_NonMoveAssign(FactoryImpl)
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_FACTORY_H_
