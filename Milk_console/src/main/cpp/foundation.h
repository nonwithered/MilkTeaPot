#ifndef MILK_CONSOLE_FOUNDATION_H_
#define MILK_CONSOLE_FOUNDATION_H_

#include <milk_console/common.h>

namespace Milk_Console {

class FoundationWrapper final : public BaseFoundation {
  static constexpr char TAG[] = "Milk_Console::FoundationWrapper";
  using raw_type = Milk_Console_Foundation_t;
  using interface_type = Milk_Console_Foundation_Interface_t;
 public:
  FoundationWrapper(raw_type another = {}) : obj_(another) {}
  raw_type ToRawType() final {
    return obj_;
  }
  SoyBean_Factory_t GetSoyBeanFactory() final {
    SoyBean_Factory_t result = {};
    MilkTea_invoke_panic(interface().GetSoyBeanFactory, obj(), &result);
    return result;
  }
 private:
  void *obj() const {
    return get().obj_;
  }
  const interface_type &interface() const {
    return *get().interface_;
  }
  const raw_type &get() const {
    return obj_;
  }
  const raw_type obj_;
};


} // namespace Milk_Console

#endif // ifndef MILK_CONSOLE_FOUNDATION_H_
