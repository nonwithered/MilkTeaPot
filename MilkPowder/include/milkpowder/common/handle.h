#ifndef LIB_SOYBEAN_COMMON_HANDLE_H_
#define LIB_SOYBEAN_COMMON_HANDLE_H_

#include <milkpowder/core.h>

namespace SoyBean {

class BaseHandle {
  using raw_type = SoyBean_Handle_t;
  using interface_type = SoyBean_Handle_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .obj_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseHandle() = default;
  virtual BaseHandle &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void SendMessage(uint8_t type, uint8_t arg0, uint8_t arg1) = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_COMMON_HANDLE_H_
