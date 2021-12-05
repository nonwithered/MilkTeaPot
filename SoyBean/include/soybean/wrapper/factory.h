#ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
#define LIB_SOYBEAN_WRAPPER_FACTORY_H_

#include <soybean/wrapper/handle.h>

namespace SoyBean {

class BaseFactory {
 public:
  virtual SoyBean_Factory_t ToRawType() && {
    return SoyBean_Factory_t{
      .self_ = &std::forward<BaseFactory>(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseFactory() = default;
  virtual BaseFactory &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual BaseHandle &Create() = 0;
 private:
  static MilkTea_decl(const SoyBean_Factory_Interface_t &) Interface();
};

class FactoryWrapper final : public BaseFactory {
  static constexpr char TAG[] = "SoyBean::FactoryWrapper";
 public:
  SoyBean_Factory_t ToRawType() && final {
    return release();
  }
  FactoryWrapper(SoyBean_Factory_t another = {}) : self_(another) {}
  FactoryWrapper(FactoryWrapper &&another) : FactoryWrapper() {
    std::swap(self_, another.self_);
  }
  ~FactoryWrapper() final {
    if (self_.self_ == nullptr) {
      return;
    }
    MilkTea_panic(SoyBean_Factory_Destroy(self_));
    self_ = {};
  }
  BaseFactory &Move() && final {
    return *new FactoryWrapper(std::forward<FactoryWrapper>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  BaseHandle &Create() final {
    return *new HandleWrapper(make_handle());
  }
  HandleWrapper make_handle() {
    SoyBean_Handle_t handle{};
    MilkTea_panic(SoyBean_Handle_Create(&handle, self_));
    return handle;
  }
  SoyBean_Factory_t release() {
    SoyBean_Factory_t self = self_;
    self_ = {};
    return self;
  }
private:
  SoyBean_Factory_t self_;
  MilkTea_NonCopy(FactoryWrapper)
  MilkTea_NonMoveAssign(FactoryWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
