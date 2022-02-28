#ifndef LIB_SOYMILK_PLAYER_H_
#define LIB_SOYMILK_PLAYER_H_

#ifdef __cplusplus
#include <functional>
#include <memory>
#include <chrono>
#include <tea/func.h>
#include <tea/err.h>
#endif // ifdef __cplusplus

#include <tea/def.h>
#include <milkpowder.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyMilk_FrameData_t;

extern
TEA_API uint64_t (TEA_CALL *
SoyMilk_FrameData_GetTime)(const struct SoyMilk_FrameData_t *);

struct SoyMilk_FrameData_recv_t {
  struct SoyMilk_FrameData_recv_capture_t *capture;
  void (TEA_CALL *invoke)(struct SoyMilk_FrameData_recv_capture_t *,
                          size_t midi_idx, uint16_t track_idx, uint32_t message_idx,
                          const struct MilkPowder_Message_t *);
};

extern
TEA_API void (TEA_CALL *
SoyMilk_FrameData_GetData)(const struct SoyMilk_FrameData_t *, struct SoyMilk_FrameData_recv_t);

enum SoyMilk_Player_State_t {
  SoyMilk_Player_State_INIT,
  SoyMilk_Player_State_PREPARING,
  SoyMilk_Player_State_PREPARED,
  SoyMilk_Player_State_SUSPEND,
  SoyMilk_Player_State_PLAYING,
  SoyMilk_Player_State_SEEKING,
  SoyMilk_Player_State_STARTED,
  SoyMilk_Player_State_RESUMED,
  SoyMilk_Player_State_PAUSED,
  SoyMilk_Player_State_STOPPED,
  SoyMilk_Player_State_RESET
};

/*
 * PREPARING -> PREPARED uint64_t OnPrepare
 * STARTED -> SUSPEND OnStart
 * RESUMED -> PLAYING OnResume
 * PAUSED -> SUSPEND uint64_t OnPause
 * STOPPED -> PREPARED OnStop
 * RESET -> INIT OnReset
 * PLAYING -> PREPARED OnComplete
 * SUSPEND -> SEEKING OnSeekBegin
 * SEEKING -> SUSPEND uint64_t OnSeekEnd
 */
struct SoyMilk_Player_OnStateChange_recv_t {
  struct SoyMilk_Player_OnStateChange_recv_ctx_t *ctx;
  void (TEA_CALL *close)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnPrepare)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *, uint64_t);
  void (TEA_CALL *OnStart)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnResume)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnPause)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *, uint64_t);
  void (TEA_CALL *OnStop)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnReset)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnComplete)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnSeekBegin)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *);
  void (TEA_CALL *OnSeekEnd)(struct SoyMilk_Player_OnStateChange_recv_ctx_t *, uint64_t);
};

struct SoyMilk_Player_OnRenderFrame_recv_t {
  struct SoyMilk_Player_OnRenderFrame_recv_capture_t *capture;
  void (TEA_CALL *close)(struct SoyMilk_Player_OnRenderFrame_recv_capture_t *);
  void (TEA_CALL *invoke)(struct SoyMilk_Player_OnRenderFrame_recv_capture_t *, const struct SoyMilk_FrameData_t *);
};

struct SoyMilk_Player_Action_t {
  struct SoyMilk_Player_Action_capture_t *capture;
  void (TEA_CALL *close)(struct SoyMilk_Player_Action_capture_t *);
  void (TEA_CALL *invoke)(struct SoyMilk_Player_Action_capture_t *);
};

struct SoyMilk_Player_Executor_t {
  struct SoyMilk_Player_Executor_capture_t *capture;
  void (TEA_CALL *close)(struct SoyMilk_Player_Executor_capture_t *);
  void (TEA_CALL *invoke)(struct SoyMilk_Player_Executor_capture_t *, struct SoyMilk_Player_Action_t);
};

struct SoyMilk_Player_Scheduled_t {
  struct SoyMilk_Player_Scheduled_capture_t *capture;
  void (TEA_CALL *close)(struct SoyMilk_Player_Scheduled_capture_t *);
  void (TEA_CALL *invoke)(struct SoyMilk_Player_Scheduled_capture_t *, struct SoyMilk_Player_Action_t, uint64_t);
};

struct SoyMilk_Player_t;

// INIT
extern
TEA_API struct SoyMilk_Player_t * (TEA_CALL *
SoyMilk_Player_Create)(struct SoyMilk_Player_OnStateChange_recv_t,
                       struct SoyMilk_Player_OnRenderFrame_recv_t,
                       struct SoyMilk_Player_Executor_t main_post,
                       struct SoyMilk_Player_Scheduled_t renderer_post);

extern
TEA_API void (TEA_CALL *
SoyMilk_Player_Destroy)(struct SoyMilk_Player_t *);

extern
TEA_API enum SoyMilk_Player_State_t (TEA_CALL *
SoyMilk_Player_GetState)(struct SoyMilk_Player_t *);

// INIT -> PREPARING
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Prepare)(struct SoyMilk_Player_t *, struct MilkPowder_Midi_t *[], size_t, struct SoyMilk_Player_Executor_t);

// PREPARED -> STARTED
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Start)(struct SoyMilk_Player_t *);

// SUSPEND -> RESUMED
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Resume)(struct SoyMilk_Player_t *);

// PLAYING -> PAUSED
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Pause)(struct SoyMilk_Player_t *);

// SUSPEND -> STOPPED
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Stop)(struct SoyMilk_Player_t *);

// PREPARED -> RESET
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Reset)(struct SoyMilk_Player_t *);

// SUSPEND -> SEEKING
extern
TEA_API bool (TEA_CALL *
SoyMilk_Player_Seek)(struct SoyMilk_Player_t *, uint64_t);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

namespace SoyMilk {

using FrameData = SoyMilk_FrameData_t;
using Player = SoyMilk_Player_t;
using duration_type = std::chrono::microseconds;
using clock_type = std::chrono::system_clock;
using time_point_type = std::chrono::time_point<clock_type, duration_type>;
using duration_type = SoyMilk::duration_type;
using action_type = std::function<void()>;
using executor_type = std::function<void(action_type)>;
using scheduled_type = std::function<void(action_type, duration_type)>;
using State_t = SoyMilk_Player_State_t;

inline
time_point_type Now() {
  return std::chrono::time_point_cast<duration_type>(clock_type::now());
}

class OnPlayerStateChangeListener : tea::remove_copy {
 public:
  explicit OnPlayerStateChangeListener(OnPlayerStateChangeListener * proxy = nullptr)
  : proxy_(proxy) {}
  virtual ~OnPlayerStateChangeListener() = default;
  virtual auto OnPrepare(duration_type time) -> void {
    if (proxy_) {
      proxy_->OnPrepare(time);
    }
  }
  virtual auto OnStart() -> void {
    if (proxy_) {
      proxy_->OnStart();
    }
  }
  virtual auto OnResume() -> void {
    if (proxy_) {
      proxy_->OnResume();
    }
  }
  virtual auto OnPause(duration_type time) -> void {
    if (proxy_) {
      proxy_->OnPause(time);
    }
  }
  virtual auto OnStop() -> void {
    if (proxy_) {
      proxy_->OnStop();
    }
  }
  virtual auto OnReset() -> void {
    if (proxy_) {
      proxy_->OnReset();
    }
  }
  virtual auto OnComplete() -> void {
    if (proxy_) {
      proxy_->OnComplete();
    }
  }
  virtual auto OnSeekBegin() -> void {
    if (proxy_) {
      proxy_->OnSeekBegin();
    }
  }
  virtual auto OnSeekEnd(duration_type time) -> void {
    if (proxy_) {
      proxy_->OnSeekEnd(time);
    }
  }
 private:
  std::unique_ptr<OnPlayerStateChangeListener> proxy_;
};

} // namespace SoyMilk

template<>
struct tea::meta::cast_pair<SoyMilk::OnPlayerStateChangeListener> {
  using type = SoyMilk_Player_OnStateChange_recv_ctx_t;
};

template<>
struct tea::meta::func_sign<SoyMilk_FrameData_recv_t> {
  using type = void(size_t, uint16_t, uint32_t, const MilkPowder::Message *);
};

template<>
struct tea::meta::func_sign<SoyMilk_Player_OnRenderFrame_recv_t> {
  using type = void(SoyMilk::FrameData *);
};

template<>
struct tea::meta::func_sign<SoyMilk_Player_Action_t> {
  using type = void();
};

template<>
struct tea::meta::func_sign<SoyMilk_Player_Executor_t> {
  using type = void(SoyMilk_Player_Action_t);
};

template<>
struct tea::meta::func_sign<SoyMilk_Player_Scheduled_t> {
  using type = void(SoyMilk_Player_Action_t, uint64_t);
};

struct SoyMilk_FrameData_t : tea::mask_type<SoyMilk::FrameData> {
  auto drop() && -> void {
    SoyMilk_FrameData_Destroy(get());
  }
  auto time() const -> uint64_t {
    return SoyMilk_FrameData_GetTime(get());
  }
  auto data(std::function<void(size_t, uint16_t, uint32_t, const MilkPowder::Message *)> func) const -> void {
    SoyMilk_FrameData_GetData(get(), tea::meta::function_cast<SoyMilk_FrameData_recv_t>(func));
  }
};

struct SoyMilk_Player_t : tea::mask_type<SoyMilk::Player> {
 private:
  template<void (SoyMilk::OnPlayerStateChangeListener:: *func)(SoyMilk::duration_type)>
  static
  auto OnListen(SoyMilk_Player_OnStateChange_recv_ctx_t *ctx, uint64_t time) -> void {
    auto &listener = *reinterpret_cast<SoyMilk::OnPlayerStateChangeListener *>(ctx);
    auto position = SoyMilk::duration_type(static_cast<int64_t>(time));
    (listener.*func)(position);
  }
 public:
  struct State : tea::empty_class {
    static inline constexpr auto INIT = SoyMilk_Player_State_INIT;
    static inline constexpr auto PREPARING = SoyMilk_Player_State_PREPARING;
    static inline constexpr auto PREPARED = SoyMilk_Player_State_PREPARED;
    static inline constexpr auto SUSPEND = SoyMilk_Player_State_SUSPEND;
    static inline constexpr auto PLAYING = SoyMilk_Player_State_PLAYING;
    static inline constexpr auto SEEKING = SoyMilk_Player_State_SEEKING;
    static inline constexpr auto STARTED = SoyMilk_Player_State_STARTED;
    static inline constexpr auto RESUMED = SoyMilk_Player_State_RESUMED;
    static inline constexpr auto PAUSED = SoyMilk_Player_State_PAUSED;
    static inline constexpr auto STOPPED = SoyMilk_Player_State_STOPPED;
    static inline constexpr auto RESET = SoyMilk_Player_State_RESET;
  };
  static
  auto init(std::unique_ptr<SoyMilk::OnPlayerStateChangeListener> listener,
            std::function<void(SoyMilk::FrameData *&)> render,
            SoyMilk::executor_type main_post,
            SoyMilk::scheduled_type renderer_post) -> SoyMilk::Player * {
    using ctx_t = SoyMilk_Player_OnStateChange_recv_ctx_t;
    using listener_t = SoyMilk::OnPlayerStateChangeListener;
    using namespace tea::meta;
    auto recv = SoyMilk_Player_OnStateChange_recv_t {
      .ctx = reinterpret_cast<ctx_t *>(listener.release()),
      .close = Class<listener_t>::destroy,
      .OnPrepare = function_handle<ctx_t *, uint64_t>::invoke<&OnListen<&listener_t::OnPrepare>>,
      .OnStart = method_handle<>::invoke<&listener_t::OnStart>,
      .OnResume = method_handle<>::invoke<&listener_t::OnResume>,
      .OnPause = function_handle<ctx_t *, uint64_t>::invoke<&OnListen<&listener_t::OnPause>>,
      .OnStop = method_handle<>::invoke<&listener_t::OnStop>,
      .OnReset = method_handle<>::invoke<&listener_t::OnReset>,
      .OnComplete = method_handle<>::invoke<&listener_t::OnComplete>,
      .OnSeekBegin = method_handle<>::invoke<&listener_t::OnSeekBegin>,
      .OnSeekEnd = function_handle<ctx_t *, uint64_t>::invoke<&OnListen<&listener_t::OnSeekEnd>>,
    };
    std::function<void(SoyMilk::FrameData *)> func_render =
    [render_frame = std::move(render)](SoyMilk::FrameData *ptr) {
      tea::defer d = [&ptr]() {
        tea::drop(ptr);
      };
      render_frame(ptr);
    };
    std::function<void(SoyMilk_Player_Action_t)> func_main_post =
    [post = std::move(main_post)](SoyMilk_Player_Action_t action) {
      post([action, close = std::shared_ptr<void>(action.capture, action.close)]() {
        action.invoke(action.capture);
      });
    };
    std::function<void(SoyMilk_Player_Action_t, uint64_t)> func_renderer_post =
    [post = std::move(renderer_post)](SoyMilk_Player_Action_t action, uint64_t delay) {
      post([action, close = std::shared_ptr<void>(action.capture, action.close)]() {
        action.invoke(action.capture);
      }, SoyMilk::duration_type(static_cast<int64_t>(delay)));
    };
    return SoyMilk_Player_Create(recv,
                                 closure_cast<SoyMilk_Player_OnRenderFrame_recv_t>(std::move(func_render)),
                                 closure_cast<SoyMilk_Player_Executor_t>(std::move(func_main_post)),
                                 closure_cast<SoyMilk_Player_Scheduled_t>(std::move(func_renderer_post)));
  }
  auto drop() && -> void {
    SoyMilk_Player_Destroy(get());
  }
  auto state() -> State {
    return SoyMilk_Player_GetState(get());
  }
  auto Prepare(tea::slice<tea::owner_ptr<MilkPowder::Midi>> arr, SoyMilk::executor_type async_post) -> bool {
    if (arr.data() == nullptr) {
      tea::err::raise<tea::err_enum::null_obj>("player prepare but array is nullptr");
      return false;
    }
    if (arr.empty()) {
      tea::err::raise<tea::err_enum::invalid_param>("player prepare but array is empty");
      return false;
    }
    std::function<void(SoyMilk_Player_Action_t)> func_async_post =
    [post = std::move(async_post)](SoyMilk_Player_Action_t action) {
      post([action, close = std::shared_ptr<void>(action.capture, action.close)]() {
        action.invoke(action.capture);
      });
    };
    auto count = arr.size();
    std::vector<MilkPowder::Midi *> vec(count);
    for (size_t i = 0; i != count; ++i) {
      vec[i] = arr[i].release();
    }
    return SoyMilk_Player_Prepare(get(), vec.data(), count, tea::meta::closure_cast<SoyMilk_Player_Executor_t>(std::move(func_async_post)));
  }
  auto Start() -> bool {
    return SoyMilk_Player_Start(get());
  }
  auto Resume() -> bool {
    return SoyMilk_Player_Resume(get());
  }
  auto Pause() -> bool {
    return SoyMilk_Player_Pause(get());
  }
  auto Stop() -> bool {
    return SoyMilk_Player_Stop(get());
  }
  auto Reset() -> bool {
    return SoyMilk_Player_Reset(get());
  }
  auto Seek(SoyMilk::duration_type position) -> bool {
    return SoyMilk_Player_Seek(get(), position.count());
  }
};

#endif // ifdef __cplusplus

#endif // LIB_SOYMILK_PLAYER_H_
