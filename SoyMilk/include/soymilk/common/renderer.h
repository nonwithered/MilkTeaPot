#ifndef LIB_SOYMILK_COMMON_RENDERER_H_
#define LIB_SOYMILK_COMMON_RENDERER_H_

#include <chrono>
#include <functional>

#include <soymilk/common/frame.h>

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
  virtual void OnFrame(Codec::FrameBufferWrapper fbo) = 0;
  virtual void OnPrepare(duration_type time) = 0;
  virtual void OnStart() = 0;
  virtual void OnResume() = 0;
  virtual void OnSeekBegin() = 0;
  virtual void OnSeekEnd(duration_type time) = 0;
  virtual void OnPause(duration_type time) = 0;
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
  raw_type ToRawType() && final {
    return release();
  }
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
  void OnFrame(Codec::FrameBufferWrapper fbo) final {
    GetInterface().OnFrame(get(), fbo.get());
  }
  void OnPrepare(duration_type time) final {
    GetInterface().OnPrepare(get(), time.count());
  }
  void OnStart() final {
    GetInterface().OnStart(get());
  }
  void OnResume() final {
    GetInterface().OnResume(get());
  }
  void OnSeekBegin() final {
    GetInterface().OnSeekBegin(get());
  }
  void OnSeekEnd(duration_type time) final {
    GetInterface().OnSeekEnd(get(), time.count());
  }
  void OnPause(duration_type time) final {
    GetInterface().OnPause(get(), time.count());
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
  raw_type release() {
    raw_type self = self_;
    self_ = {};
    return self;
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

class RendererProxy final : public BaseRenderer {
  using duration_type = TeaPot::TimerUnit::duration_type;
  using base_type = BaseRenderer;
  using self_type = RendererProxy;
 public:
  RendererProxy(base_type &another) : self_(another) {}
  RendererProxy(const self_type &another) : self_(another.self_) {}
  RendererProxy(self_type &&another) :self_(another.self_) {}
 public:
  ~RendererProxy() final {
  }
  base_type *Move() && final {
    return new self_type(std::forward<self_type>(*this));
  }
  void Destroy() && final {
    delete this;
  }
  void OnFrame(Codec::FrameBufferWrapper fbo) final {
    get().OnFrame(std::move(fbo));
  }
  void OnPrepare(duration_type time) final {
    get().OnPrepare(time);
  }
  void OnStart() final {
    get().OnStart();
  }
  void OnResume() final {
    get().OnResume();
  }
  void OnSeekBegin() final {
    get().OnSeekBegin();
  }
  void OnSeekEnd(duration_type time) final {
    get().OnSeekEnd(time);
  }
  void OnPause(duration_type time) final {
    get().OnPause(time);
  }
  void OnStop() final {
    get().OnStop();
  }
  void OnReset() final {
    get().OnReset();
  }
  void OnComplete() final {
    get().OnComplete();
  }
 private:
  base_type &get() {
    return self_;
  }
  base_type &self_;
};

} // namespace SoyMilk

#endif // ifndef LIB_SOYMILK_COMMON_RENDERER_H_
