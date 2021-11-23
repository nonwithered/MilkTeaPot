#ifndef LIB_SOYMILK_CORE_H_
#define LIB_SOYMILK_CORE_H_

#include <milkpowder.h>

#ifdef __cplusplus
extern "C" {
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

struct SoyMilk_Player_Controller_Interface_t {
  void (MilkTea_CALL *Deleter)(void *self);
  void (MilkTea_CALL *OnSubmit)(void *self, MilkTea_ClosureToken_t obj, void (MilkTea_CALL *submit)(void *obj));
  void (MilkTea_CALL *OnPlay)(void *self, int64_t time, uint16_t ntrk, MilkPowder_Message_t *message);
  void (MilkTea_CALL *OnPrepare)(void *self, int64_t time);
  void (MilkTea_CALL *OnStart)(void *self);
  void (MilkTea_CALL *OnPause)(void *self, int64_t time);
  void (MilkTea_CALL *OnSeekBegin)(void *self, int64_t time);
  void (MilkTea_CALL *OnSeekEnd)(void *self);
  void (MilkTea_CALL *OnResume)(void *self);
  void (MilkTea_CALL *OnStop)(void *self, int64_t time);
  void (MilkTea_CALL *OnComplete)(void *self);
  void (MilkTea_CALL *OnReset)(void *self);
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Controller_Interface_t SoyMilk_Player_Controller_Interface_t;
#endif

struct SoyMilk_Player_Controller_t {
  void *self_;
  const SoyMilk_Player_Controller_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Callback_t SoyMilk_Player_Callback_t;
#endif

struct SoyMilk_Player_t;

#ifndef __cplusplus
typedef struct SoyMilk_Player_t SoyMilk_Player_t;
#endif

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Create(SoyMilk_Player_t **self, MilkTea_TimerWorker_Weak_t *timer, SoyMilk_Player_Controller_t callback);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Destroy(SoyMilk_Player_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Prepare(SoyMilk_Player_t *self, const MilkPowder_Midi_t *midi);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Start(SoyMilk_Player_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Pause(SoyMilk_Player_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Seek(SoyMilk_Player_t *self, int64_t time);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Resume(SoyMilk_Player_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Stop(SoyMilk_Player_t *self);

MilkTea_API MilkTea_Exception_t MilkTea_CALL
SoyMilk_Player_Reset(SoyMilk_Player_t *self);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_H_
