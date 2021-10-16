#ifndef LIB_SOYMILK_CORE_H_
#define LIB_SOYMILK_CORE_H_

#include <soybean.h>

struct SoyMilk_Player_t;

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

#endif // ifndef LIB_SOYMILK_CORE_H_
