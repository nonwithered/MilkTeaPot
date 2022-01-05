#ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
#define LIB_SOYBEAN_WRAPPER_FACTORY_H_

#include <soybean/wrapper/handle.h>

namespace SoyBean {

class BaseFactory {
  using raw_type = SoyBean_Factory_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
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
  using raw_type = SoyBean_Factory_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  FactoryWrapper(raw_type another = {}) : self_(another) {}
  FactoryWrapper(FactoryWrapper &&another) : FactoryWrapper() {
    std::swap(self_, another.self_);
  }
  ~FactoryWrapper() final {
    if (self_.self_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyBean_Factory_Destroy, self_);
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
    MilkTea_invoke_panic(SoyBean_Handle_Create, &handle, self_);
    return handle;
  }
  raw_type release() {
    raw_type self = self_;
    self_ = {};
    return self;
  }
private:
  raw_type self_;
  MilkTea_NonCopy(FactoryWrapper)
  MilkTea_NonMoveAssign(FactoryWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_FACTORY_H_
