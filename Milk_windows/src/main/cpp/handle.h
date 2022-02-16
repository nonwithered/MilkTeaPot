#ifndef MILK_WINDOWS_HANDLE_H_
#define MILK_WINDOWS_HANDLE_H_

#include <array>
#include <functional>

#include <milkpowder.h>

namespace Milk_Windows {

class HandleImpl final : public SoyBean::BaseHandle {
  static constexpr size_t msg_size = 4;
 public:
  explicit HandleImpl(std::function<void(uint8_t, uint8_t, uint8_t, uint8_t)> consumer) : consumer_(std::move(consumer)) {}
  HandleImpl(HandleImpl &&another) : consumer_(std::move(another.consumer_)) {}
  ~HandleImpl() = default;
  HandleImpl &Move() && final {
    return *new HandleImpl(std::move(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void SendMessage(uint8_t type, uint8_t arg0, uint8_t arg1) final {
    SendMsg(0x80, type, arg0, arg1);
  }
 private:
  void SendMsg(uint8_t type, uint8_t channel, uint8_t arg0, uint8_t arg1 = 0) {
    if (!consumer_) {
      return;
    }
    consumer_(type, channel, arg0, arg1);
  }
  std::function<void(uint8_t, uint8_t, uint8_t, uint8_t)> consumer_;
  MilkTea_NonCopy(HandleImpl)
  MilkTea_NonMoveAssign(HandleImpl)
};

} // namespace Milk_Windows

#endif // ifndef MILK_WINDOWS_HANDLE_H_
