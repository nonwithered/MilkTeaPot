#ifndef LIB_SOYMILK_PLAYER_H_
#define LIB_SOYMILK_PLAYER_H_

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
SoyMilk_FrameData_GetTimeStamp)(const struct SoyMilk_FrameData_t *);

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
 * PREPARING -> PREPARED uint64_t
 * STARTED -> SUSPEND
 * RESUMED -> PLAYING
 * PAUSED -> SUSPEND uint64_t
 * STOPPED -> PREPARED
 * RESET -> INIT
 * SEEKING -> SUSPEND uint64_t
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
  void (TEA_CALL *invoke)(struct SoyMilk_Player_Executor_capture_t *, struct SoyMilk_Player_Action_t, uint64_t);
};

struct SoyMilk_Player_t;

// INIT
extern
TEA_API struct SoyMilk_Player_t * (TEA_CALL *
SoyMilk_Player_Create)(struct SoyMilk_Player_OnStateChange_recv_t,
                       struct SoyMilk_Player_OnRenderFrame_recv_t,
                       struct SoyMilk_Player_Executor_t main_post,
                       struct SoyMilk_Player_Executor_t renderer_post);

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

#endif // LIB_SOYMILK_PLAYER_H_
