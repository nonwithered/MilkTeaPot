#ifndef MILK_WINDOWS_FACTORY_H_
#define MILK_WINDOWS_FACTORY_H_

#include "handle.h"
#include "remote.h"

namespace Milk_Windows {

class FactoryImpl final : public SoyBean::BaseFactory {
 public:
  explicit FactoryImpl(std::string_view work_path) : work_path_(work_path) {}
  FactoryImpl(FactoryImpl &&another) : work_path_(another.work_path_) {}
  ~FactoryImpl() = default;
  FactoryImpl &Move() && final {
    return *new FactoryImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  SoyBean_Handle_t Create() final {
    return HandleImpl([remote = RemoteImpl::Make(work_path_)](auto type, auto channel, auto arg0, auto arg1) {
      if (!remote) {
        return;
      }
      remote->SendMsg(type, channel, arg0, arg1);
    }).ToRawType();
  }
 private:
  std::string_view work_path_;
  MilkTea_NonCopy(FactoryImpl)
  MilkTea_NonMoveAssign(FactoryImpl)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_FACTORY_H_
