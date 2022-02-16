#ifndef LIB_SOYBEAN_WRAPPER_HANDLE_H_
#define LIB_SOYBEAN_WRAPPER_HANDLE_H_

#include <milkpowder/common.h>

namespace SoyBean {

class HandleWrapper final : public BaseHandle {
  static constexpr char TAG[] = "SoyBean::HandleWrapper";
  using raw_type = SoyBean_Handle_t;
 public:
  raw_type ToRawType() && final {
    return release();
  }
  HandleWrapper(raw_type another = {}) : obj_(another) {}
  HandleWrapper(HandleWrapper &&another) : HandleWrapper(another.release()) {}
  ~HandleWrapper() final {
    auto obj = release();
    if (obj.obj_ == nullptr) {
      return;
    }
    MilkTea_invoke_panic(SoyBean_Handle_Destroy, obj);
  }
  BaseHandle &Move() && final {
    return *new HandleWrapper(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void SendMessage(uint8_t type, uint8_t arg0, uint8_t arg1) final {
    MilkTea_invoke_panic(SoyBean_Handle_SendMessage, get(), type, arg0, arg1);
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
  MilkTea_NonCopy(HandleWrapper)
  MilkTea_NonMoveAssign(HandleWrapper)
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_WRAPPER_HANDLE_H_
