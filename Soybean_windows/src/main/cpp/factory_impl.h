#ifndef SOYBEAN_WINDOWS_FACTORY_H_
#define SOYBEAN_WINDOWS_FACTORY_H_

#include <soybean.h>

#include "handle_impl.h"

namespace SoyBean_Windows {

class FactoryImpl final : public SoyBean::BaseFactory {
 public:
  static SoyBean::BaseFactory &Instance();
  SoyBean::BaseHandle *Create() final {
    return new HandleImpl();
  }
};

} // namespace SoyBean_Windows

#endif // ifndef SOYBEAN_WINDOWS_FACTORY_H_
