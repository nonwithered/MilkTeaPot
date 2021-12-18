#ifndef LIB_SOYMILK_COMMON_RENDERER_H_
#define LIB_SOYMILK_COMMON_RENDERER_H_

#include <chrono>
#include <functional>

#include <soymilk/common.h>

namespace SoyMilk {

class BaseRenderer {
  using raw_type = SoyMilk_Player_Renderer_t;
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .self_ = std::forward<BaseRenderer>(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseRenderer() = default;
  virtual BaseRenderer *Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void OnRender(duration_type time, uint16_t ntrk, MilkPowder::MessageMutableWrapper message) = 0;
  virtual void OnPrepare(duration_type time) = 0;
  virtual void OnStart() = 0;
  virtual void OnPause(duration_type time) = 0;
  virtual void OnSeekBegin() = 0;
  virtual void OnSeekEnd() = 0;
  virtual void OnResume() = 0;
  virtual void OnStop() = 0;
  virtual void OnReset() = 0;
  virtual void OnComplete() = 0;
 private:
  static MilkTea_decl(const SoyMilk_Player_Renderer_Interface_t &) Interface();
};

class RendererWrapper final : public BaseRenderer {
  using raw_type = SoyMilk_Player_Renderer_t;
  using duration_type = TeaPot::TimerUnit::duration_type;
 public:
  RendererWrapper(raw_type another = {}) : self_(another) {}
  RendererWrapper(RendererWrapper &&another) : RendererWrapper() {
    std::swap(self_, another.self_);
  }
  ~RendererWrapper() final {
    if (get() == nullptr) {
      return;
    }
    GetInterface().Deleter(get());
    self_ = {};
  }
  BaseRenderer *Move() && final {
    return new RendererWrapper(std::forward<RendererWrapper>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void OnRender(duration_type time, uint16_t ntrk, MilkPowder::MessageMutableWrapper message) final {
    GetInterface().OnRender(get(), time.count(), ntrk, message.release());
  }
  void OnPrepare(duration_type time) final {
    GetInterface().OnPrepare(get(), time.count());
  }
  void OnStart() final {
    GetInterface().OnStart(get());
  }
  void OnPause(duration_type time) final {
    GetInterface().OnPause(get(), time.count());
  }
  void OnSeekBegin() final {
    GetInterface().OnSeekBegin(get());
  }
  void OnSeekEnd() final {
    GetInterface().OnSeekEnd(get());
  }
  void OnResume() final {
    GetInterface().OnResume(get());
  }
  void OnStop() final {
    GetInterface().OnStop(get());
  }
  void OnReset() final {
    GetInterface().OnReset(get());
  }
  void OnComplete() final {
    GetInterface().OnComplete(get());
  }
 private:
  void *get() const {
    return self_.self_;
  }
  const SoyMilk_Player_Renderer_Interface_t &GetInterface() const {
    return *self_.interface_;
  }
  raw_type self_;
  MilkTea_NonCopy(RendererWrapper)
  MilkTea_NonMoveAssign(RendererWrapper)
};

} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_RENDERER_H_
