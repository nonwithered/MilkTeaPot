#ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
#define LIB_SOYBEAN_WRAPPER_FACTORY_H_

#include <milkpowder/common.h>

namespace SoyBean {

class FactoryWrapper final : public BaseFactory {
  static constexpr char TAG[] = "SoyBean::FactoryWrapper";
  using raw_type = SoyBean_Factory_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  FactoryWrapper(raw_type another = {}) : obj_(another) {}
  FactoryWrapper(FactoryWrapper &&another) : FactoryWrapper(another.release()) {}
  ~FactoryWrapper() final {
    auto obj = release();
    if (obj.obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyBean_Factory_Destroy, obj);
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
    std::swap(obj_, result);
    return result;
  }
  raw_type get() {
    return obj_;
  }
private:
  raw_type obj_;
  MilkTea_NonCopy(FactoryWrapper)
  MilkTea_NonMoveAssign(FactoryWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
