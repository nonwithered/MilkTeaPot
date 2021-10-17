#ifndef LIB_SOYMILK_CORE_H_
#define LIB_SOYMILK_CORE_H_

#include <milktea.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyMilk_Player_t;

#ifndef __cplusplus
typedef struct SoyMilk_Player_t SoyMilk_Player_t;
#endif

enum SoyMilk_Player_State_t {
  SoyMilk_Player_State_Error,
  SoyMilk_Player_State_Idle,
  SoyMilk_Player_State_Initialized,
  SoyMilk_Player_State_Preparing,
  SoyMilk_Player_State_Prepared,
  SoyMilk_Player_State_Started,
  SoyMilk_Player_State_Paused,
  SoyMilk_Player_State_Stopped,
  SoyMilk_Player_State_PlaybackComplete
};

#ifndef __cplusplus
typedef enum SoyMilk_Player_State_t SoyMilk_Player_State_t;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_H_
