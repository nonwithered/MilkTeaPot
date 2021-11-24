#ifndef SOYBEAN_WINDOWS_FACTORY_H_
#define SOYBEAN_WINDOWS_FACTORY_H_

#include <soybean.h>

#include "handle.h"

namespace SoyBean_Windows {

class FactoryImpl final : public SoyBean::BaseFactory {
 public:
  static std::unique_ptr<FactoryImpl> Make(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen) {
    return std::unique_ptr<FactoryImpl>(new FactoryImpl(uDeviceID, dwCallback, dwInstance, fdwOpen));
  }
  ~FactoryImpl() final {
    uDeviceID_ = 0;
    dwCallback_ = nullptr;
    dwInstance_ = nullptr;
    fdwOpen_ = 0;
  }
  void Destroy() && final {
    delete this;
  }
  std::unique_ptr<SoyBean::BaseHandle> Create() final {
    return HandleImpl::Make(uDeviceID_, dwCallback_, dwInstance_, fdwOpen_);
  }
 private:
  FactoryImpl(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen)
  : uDeviceID_(uDeviceID),
    dwCallback_(dwCallback),
    dwInstance_(dwInstance),
    fdwOpen_(fdwOpen) {}
  unsigned int uDeviceID_;
  uint32_t *dwCallback_;
  uint32_t *dwInstance_;
  uint32_t fdwOpen_;
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_FACTORY_H_
