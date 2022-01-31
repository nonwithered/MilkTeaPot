#ifndef LIB_SOYBEAN_COMMON_FACTORY_H_
#define LIB_SOYBEAN_COMMON_FACTORY_H_

#include <soybean/common/handle.h>

namespace SoyBean {

class BaseFactory {
  using raw_type = SoyBean_Factory_t;
  using interface_type = SoyBean_Factory_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .self_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseFactory() = default;
  virtual BaseFactory &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual SoyBean_Handle_t Create() = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

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

#endif // ifndef LIB_SOYBEAN_COMMON_FACTORY_H_
