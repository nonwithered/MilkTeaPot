#ifndef LIB_SOYBEAN_COMMON_FACTORY_H_
#define LIB_SOYBEAN_COMMON_FACTORY_H_

#include <milkpowder/core.h>

namespace SoyBean {

class BaseFactory {
  using raw_type = SoyBean_Factory_t;
  using interface_type = SoyBean_Factory_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .obj_ = &std::move(*this).Move(),
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

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_COMMON_FACTORY_H_
