#ifndef LIB_SOYMILK_CALLBACK_H_
#define LIB_SOYMILK_CALLBACK_H_

#include <soymilk/core.h>

#include <chrono>
#include <functional>

#ifdef __cplusplus
namespace SoyMilk {
class BaseCallback {
 public:
  using duration_type = std::chrono::milliseconds;
  MilkTea_API SoyMilk_Player_Callback_t MilkTea_CALL ToRawCallback();
  virtual void OnPlay(duration_type time, uint16_t ntrk, MilkPowder::Message message) = 0;
  virtual void OnPrepare(duration_type time) = 0;
  virtual void OnStart() = 0;
  virtual void OnPause(duration_type time) = 0;
  virtual void OnSeekBegin(duration_type time) = 0;
  virtual void OnSeekEnd() = 0;
  virtual void OnResume() = 0;
  virtual void OnStop(duration_type time) = 0;
  virtual void OnComplete() = 0;
  virtual void OnCompleteSubmit(std::function<void()>) = 0;
  virtual void OnReset() = 0;
};
class CallbackWrapper final : public BaseCallback {
 public:
  explicit CallbackWrapper(const SoyMilk_Player_Callback_t &callback) : callback_(callback) {};
  void OnPlay(duration_type time, uint16_t ntrk, MilkPowder::Message message) final {
    callback_.interface_->OnPlay(callback_.self_, time.count(), ntrk, message.release());
  }
  void OnPrepare(duration_type time) final {
    callback_.interface_->OnPrepare(callback_.self_, time.count());
  }
  void OnStart() final {
    callback_.interface_->OnStart(callback_.self_);
  }
  void OnPause(duration_type time) final {
    callback_.interface_->OnPause(callback_.self_, time.count());
  }
  void OnSeekBegin(duration_type time) final {
    callback_.interface_->OnSeekBegin(callback_.self_, time.count());
  }
  void OnSeekEnd() final {
    callback_.interface_->OnSeekEnd(callback_.self_);
  }
  void OnResume() final {
    callback_.interface_->OnResume(callback_.self_);
  }
  void OnStop(duration_type time) final {
    callback_.interface_->OnStop(callback_.self_, time.count());
  }
  void OnComplete() final {
    callback_.interface_->OnComplete(callback_.self_);
  }
  void OnCompleteSubmit(std::function<void()> submit) final {
    callback_.interface_->OnCompleteSubmit(callback_.self_, &submit, MilkTea::ClosureToken<void()>::Invoke);
  };
  void OnReset() final {
    callback_.interface_->OnReset(callback_.self_);
  }
 private:
  const SoyMilk_Player_Callback_t callback_;
};
} // namespace SoyMilk
#endif

#endif // ifndef LIB_SOYMILK_CALLBACK_H_
