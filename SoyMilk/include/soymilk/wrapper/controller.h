#ifndef LIB_SOYMILK_WRAPPER_CONTROLLER_H_
#define LIB_SOYMILK_WRAPPER_CONTROLLER_H_

#include <chrono>
#include <functional>

#include <soymilk/common.h>

namespace SoyMilk {

class BaseController {
 public:
  using duration_type = std::chrono::milliseconds;
  SoyMilk_Player_Controller_t ToRawType() && {
    return SoyMilk_Player_Controller_t{
      .self_ = std::forward<BaseController>(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseController() = default;
  virtual BaseController *Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void OnSubmit(std::function<void()>) = 0;
  virtual void OnPlay(duration_type time, uint16_t ntrk, MilkPowder::MessageMutableWrapper message) = 0;
  virtual void OnPrepare(duration_type time) = 0;
  virtual void OnStart() = 0;
  virtual void OnPause(duration_type time) = 0;
  virtual void OnSeekBegin(duration_type time) = 0;
  virtual void OnSeekEnd() = 0;
  virtual void OnResume() = 0;
  virtual void OnStop(duration_type time) = 0;
  virtual void OnComplete() = 0;
  virtual void OnReset() = 0;
 private:
  static MilkTea_api const SoyMilk_Player_Controller_Interface_t & MilkTea_call Interface();
};

class ControllerWrapper final : public BaseController {
 public:
  ControllerWrapper(SoyMilk_Player_Controller_t another = {}) : self_(another) {}
  ControllerWrapper(ControllerWrapper &&another) : ControllerWrapper() {
    std::swap(self_, another.self_);
  }
  ~ControllerWrapper() final {
    if (GetObj() == nullptr) {
      return;
    }
    GetInterface().Deleter(GetObj());
    self_ = {};
  }
  BaseController *Move() && final {
    return new ControllerWrapper(std::forward<ControllerWrapper>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void OnSubmit(std::function<void()> submit) final {
    GetInterface().OnSubmit(GetObj(), MilkTea::ClosureToken<decltype(submit)>::ToRawType(submit), MilkTea::ClosureToken<decltype(submit)>::Invoke);
  };
  void OnPlay(duration_type time, uint16_t ntrk, MilkPowder::MessageMutableWrapper message) final {
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
  SoyMilk_Player_Controller_t self_;
  MilkTea_NonCopy(ControllerWrapper)
  MilkTea_NonMoveAssign(ControllerWrapper)
};

} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_WRAPPER_CONTROLLER_H_
