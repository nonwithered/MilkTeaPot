#ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
#define LIB_SOYBEAN_WRAPPER_FACTORY_H_

#include <soybean/common.h>

namespace SoyBean {

class FactoryWrapper final : public BaseFactory {
  static constexpr char TAG[] = "SoyBean::FactoryWrapper";
  using raw_type = SoyBean_Factory_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  FactoryWrapper(raw_type another = {}) : self_(another) {}
  FactoryWrapper(FactoryWrapper &&another) : FactoryWrapper(another.release()) {}
  ~FactoryWrapper() final {
    auto self = release();
    if (self.self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyBean_Factory_Destroy, self);
  }
  BaseFactory &Move() && final {
    return *new FactoryWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  SoyBean_Handle_t Create() final {
    SoyBean_Handle_t result = {};
    MilkTea_invoke_panic(SoyBean_Handle_Create, &result, get());
    return result;
  }
  raw_type release() {
    raw_type result = {};
    std::swap(self_, result);
    return result;
  }
  raw_type get() {
    return self_;
  }
private:
  raw_type self_;
  MilkTea_NonCopy(FactoryWrapper)
  MilkTea_NonMoveAssign(FactoryWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
