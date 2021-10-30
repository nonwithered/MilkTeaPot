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
  SoyMilk_Player_State_INIT,
  SoyMilk_Player_State_PREPARING,
  SoyMilk_Player_State_PREPARED,
  SoyMilk_Player_State_STARTED,
  SoyMilk_Player_State_PAUSED
};

#ifndef __cplusplus
typedef enum SoyMilk_Player_State_t SoyMilk_Player_State_t;
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_H_
