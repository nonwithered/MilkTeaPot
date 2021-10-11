#ifndef SOYBEAN_WINDOWS_FACTORY_H_
#define SOYBEAN_WINDOWS_FACTORY_H_

#include <soybean.h>

#include "handle_impl.h"

namespace SoyBean_Windows {

class FactoryImpl final : public SoyBean::BaseFactory {
 public:
  FactoryImpl(unsigned int uDeviceID, uint32_t *dwCallback, uint32_t *dwInstance, uint32_t fdwOpen)
  : uDeviceID_(uDeviceID),
    dwCallback_(dwCallback),
    dwInstance_(dwInstance),
    fdwOpen_(fdwOpen){}
  SoyBean::BaseHandle *Create() final {
    return new HandleImpl(uDeviceID_, dwCallback_, dwInstance_, fdwOpen_);
  }
 private:
  unsigned int uDeviceID_;
  uint32_t *dwCallback_;
  uint32_t *dwInstance_;
  uint32_t fdwOpen_;
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_FACTORY_H_
