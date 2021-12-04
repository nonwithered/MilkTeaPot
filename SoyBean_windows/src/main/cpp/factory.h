#ifndef SOYBEAN_WINDOWS_FACTORY_H_
#define SOYBEAN_WINDOWS_FACTORY_H_

#include <soybean.h>

#include "handle.h"

namespace SoyBean_Windows {

class FactoryImpl final : public SoyBean::BaseFactory {
 public:
  FactoryImpl(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen)
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
  BaseFactory *Move() && final {
    return new FactoryImpl(std::forward<FactoryImpl>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  std::unique_ptr<SoyBean::BaseHandle> Create() final {
    if (!enable_) {
      MilkTea_throw(LogicError, "Create but disable");
    }
    return std::make_unique<HandleImpl>(uDeviceID_, dwCallback_, dwInstance_, fdwOpen_);
  }
 private:
  FactoryImpl() : enable_(false), uDeviceID_(0), dwCallback_(nullptr), dwInstance_(nullptr), fdwOpen_(0) {}
  bool enable_;
  unsigned int uDeviceID_;
  uint32_t *dwCallback_;
  uint32_t *dwInstance_;
  uint32_t fdwOpen_;
  MilkTea_NonCopy(FactoryImpl)
  MilkTea_NonMoveAssign(FactoryImpl)
  static constexpr char TAG[] = "SoyBean_Windows::FactoryImpl";
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_FACTORY_H_
