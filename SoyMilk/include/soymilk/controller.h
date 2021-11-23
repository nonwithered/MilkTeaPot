#ifndef LIB_SOYMILK_CALLBACK_H_
#define LIB_SOYMILK_CALLBACK_H_

#include <soymilk/core.h>

#include <chrono>
#include <functional>

#ifdef __cplusplus
namespace SoyMilk {
class BaseController {
 public:
  using duration_type = std::chrono::milliseconds;
  MilkTea_API SoyMilk_Player_Controller_t MilkTea_CALL ToRawType();
  virtual void Deleter() = 0;
  virtual void OnSubmit(std::function<void()>) = 0;
  virtual void OnPlay(duration_type time, uint16_t ntrk, MilkPowder::Message message) = 0;
  virtual void OnPrepare(duration_type time) = 0;
  virtual void OnStart() = 0;
  virtual void OnPause(duration_type time) = 0;
  virtual void OnSeekBegin(duration_type time) = 0;
  virtual void OnSeekEnd() = 0;
  virtual void OnResume() = 0;
  virtual void OnStop(duration_type time) = 0;
  virtual void OnComplete() = 0;
  virtual void OnReset() = 0;
};
class ControllerWrapper final : public BaseController {
 public:
  explicit ControllerWrapper(SoyMilk_Player_Controller_t &&self) : ControllerWrapper() {
    std::swap(self_, self);
  };
  explicit ControllerWrapper(ControllerWrapper &&another) : ControllerWrapper(std::move(another.self_)) {}
  ~ControllerWrapper() {
    if (self_.self_ != nullptr) {
      Deleter();
      self_ = {};
    }
  }
  void Deleter() final {
    GetInterface().Deleter(GetObj());
  }
  void OnSubmit(std::function<void()> submit) final {
    GetInterface().OnSubmit(GetObj(), MilkTea::ClosureToken<decltype(submit)>::ToRawType(submit), MilkTea::ClosureToken<void()>::Invoke);
  };
  void OnPlay(duration_type time, uint16_t ntrk, MilkPowder::Message message) final {
    GetInterface().OnPlay(GetObj(), time.count(), ntrk, message.release());
  }
  void OnPrepare(duration_type time) final {
    GetInterface().OnPrepare(GetObj(), time.count());
  }
  void OnStart() final {
    GetInterface().OnStart(GetObj());
  }
  void OnPause(duration_type time) final {
    GetInterface().OnPause(GetObj(), time.count());
  }
  void OnSeekBegin(duration_type time) final {
    GetInterface().OnSeekBegin(GetObj(), time.count());
  }
  void OnSeekEnd() final {
    GetInterface().OnSeekEnd(GetObj());
  }
  void OnResume() final {
    GetInterface().OnResume(GetObj());
  }
  void OnStop(duration_type time) final {
    GetInterface().OnStop(GetObj(), time.count());
  }
  void OnComplete() final {
    GetInterface().OnComplete(GetObj());
  }
  void OnReset() final {
    GetInterface().OnReset(GetObj());
  }
 private:
  void *GetObj() const {
    return self_.self_;
  }
  const SoyMilk_Player_Controller_Interface_t &GetInterface() const {
    return *self_.interface_;
  }
  ControllerWrapper() : self_{} {}
  SoyMilk_Player_Controller_t self_;
  MilkTea_NonCopy(ControllerWrapper)
  MilkTea_NonMoveAssign(ControllerWrapper)
};
} // namespace SoyMilk
#endif

#endif // ifndef LIB_SOYMILK_CALLBACK_H_
