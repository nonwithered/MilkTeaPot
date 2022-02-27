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
TEA_API void (TEA_CALL *
SoyMilk_FrameData_Destroy)(struct SoyMilk_FrameData_t *);

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
  struct SoyMilk_Player_OnStateChange_recv_capture_t *capture;
  void (TEA_CALL *close)(struct SoyMilk_Player_OnStateChange_recv_capture_t *);
  void (TEA_CALL *invoke)(struct SoyMilk_Player_OnStateChange_recv_capture_t *,
                          enum SoyMilk_Player_State_t old_state, enum SoyMilk_Player_State_t new_state,
                          struct SoyMilk_Player_OnStateChange_recv_data_t *);
};

struct SoyMilk_Player_OnRenderFrame_recv_t {
  struct SoyMilk_Player_OnRenderFrame_recv_capture_t *capture;
  void (TEA_CALL *close)(struct SoyMilk_Player_OnRenderFrame_recv_capture_t *);
  void (TEA_CALL *invoke)(struct SoyMilk_Player_OnRenderFrame_recv_capture_t *, struct SoyMilk_FrameData_t *);
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

class OnPlayerStateChangeListener : tea::remove_copy {
 public:
  explicit OnPlayerStateChangeListener(OnPlayerStateChangeListener * proxy = nullptr)
  : proxy_(proxy) {}
  virtual ~OnPlayerStateChangeListener() = default;
  auto OnPrepare(duration_type time) -> void {
    if (proxy_) {
      proxy_->OnPrepare(time);
    }
  }
  auto OnStart() -> void {
    if (proxy_) {
      proxy_->OnStart();
    }
  }
  auto OnResume() -> void {
    if (proxy_) {
      proxy_->OnResume();
    }
  }
  auto OnPause(duration_type time) -> void {
    if (proxy_) {
      proxy_->OnPause(time);
    }
  }
  auto OnStop() -> void {
    if (proxy_) {
      proxy_->OnStop();
    }
  }
  auto OnReset() -> void {
    if (proxy_) {
      proxy_->OnReset();
    }
  }
  auto OnComplete() -> void {
    if (proxy_) {
      proxy_->OnComplete();
    }
  }
  auto OnSeekBegin() -> void {
    if (proxy_) {
      proxy_->OnSeekBegin();
    }
  }
  auto OnSeekEnd(duration_type time) -> void {
    if (proxy_) {
      proxy_->OnSeekEnd(time);
    }
  }
 private:
  std::unique_ptr<OnPlayerStateChangeListener> proxy_;
};

} // namespace SoyMilk

template<>
struct tea::meta::func_sign<SoyMilk_Player_OnStateChange_recv_t> {
  using type = void(SoyMilk_Player_State_t, SoyMilk_Player_State_t,
                    SoyMilk_Player_OnStateChange_recv_data_t *);
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
  static
  auto get_tempo(SoyMilk_Player_OnStateChange_recv_data_t *param) -> SoyMilk::duration_type {
    auto p = reinterpret_cast<uint64_t *>(param);
    auto i = static_cast<int64_t>(*p);
    return SoyMilk::duration_type(i);
  }
 public:
  using State = SoyMilk_Player_State_t;
  static
  auto init(std::unique_ptr<SoyMilk::OnPlayerStateChangeListener> listener,
            std::function<void(SoyMilk::FrameData *)> func_render,
            std::function<void(std::function<void()>)> main_post,
            std::function<void(std::function<void()>, SoyMilk::duration_type)> renderer_post) -> SoyMilk::Player * {
    std::function<void(State, State, SoyMilk_Player_OnStateChange_recv_data_t *)> func_listener =
    [listen = std::shared_ptr<SoyMilk::OnPlayerStateChangeListener>(listener.release())](State old_state, State new_state, SoyMilk_Player_OnStateChange_recv_data_t *param) {
      if (old_state == SoyMilk_Player_State_PREPARING && new_state == SoyMilk_Player_State_PREPARED) {
        listen->OnPrepare(get_tempo(param));
        return;
      }
      if (old_state == SoyMilk_Player_State_STARTED && new_state == SoyMilk_Player_State_SUSPEND) {
        listen->OnStart();
        return;
      }
      if (old_state == SoyMilk_Player_State_RESUMED && new_state == SoyMilk_Player_State_PLAYING) {
        listen->OnResume();
        return;
      }
      if (old_state == SoyMilk_Player_State_PAUSED && new_state == SoyMilk_Player_State_SUSPEND) {
        listen->OnPause(get_tempo(param));
        return;
      }
      if (old_state == SoyMilk_Player_State_STOPPED && new_state == SoyMilk_Player_State_PREPARED) {
        listen->OnStop();
        return;
      }
      if (old_state == SoyMilk_Player_State_RESET && new_state == SoyMilk_Player_State_INIT) {
        listen->OnReset();
        return;
      }
      if (old_state == SoyMilk_Player_State_PLAYING && new_state == SoyMilk_Player_State_PREPARED) {
        listen->OnComplete();
        return;
      }
      if (old_state == SoyMilk_Player_State_SUSPEND && new_state == SoyMilk_Player_State_SEEKING) {
        listen->OnSeekBegin();
        return;
      }
      if (old_state == SoyMilk_Player_State_SEEKING && new_state == SoyMilk_Player_State_SUSPEND) {
        listen->OnSeekEnd(get_tempo(param));
        return;
      }
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
    return SoyMilk_Player_Create(tea::meta::closure_cast<SoyMilk_Player_OnStateChange_recv_t>(std::move(func_listener)),
                                 tea::meta::closure_cast<SoyMilk_Player_OnRenderFrame_recv_t>(std::move(func_render)),
                                 tea::meta::closure_cast<SoyMilk_Player_Executor_t>(std::move(func_main_post)),
                                 tea::meta::closure_cast<SoyMilk_Player_Scheduled_t>(std::move(func_renderer_post)));
  }
  auto drop() && -> void {
    SoyMilk_Player_Destroy(get());
  }
  auto state() -> State {
    return SoyMilk_Player_GetState(get());
  }
  auto Prepare(tea::slice<MilkPowder::Midi *> arr, std::function<void(std::function<void()>)> async_post) -> bool {
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
    return SoyMilk_Player_Prepare(get(), arr.data(), arr.size(), tea::meta::closure_cast<SoyMilk_Player_Executor_t>(std::move(func_async_post)));
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
