#ifndef LIB_SOYMILK_CORE_H_
#define LIB_SOYMILK_CORE_H_

#include <milkpowder.h>
#include <teapot.h>

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
  void (MilkTea_call *Deleter)(void *self);
  void (MilkTea_call *OnSubmit)(void *self, MilkTea_ClosureToken_t obj, void (MilkTea_call *submit)(void *obj));
  void (MilkTea_call *OnPlay)(void *self, int64_t time, uint16_t ntrk, MilkPowder_Message_t *message);
  void (MilkTea_call *OnPrepare)(void *self, int64_t time);
  void (MilkTea_call *OnStart)(void *self);
  void (MilkTea_call *OnPause)(void *self, int64_t time);
  void (MilkTea_call *OnSeekBegin)(void *self, int64_t time);
  void (MilkTea_call *OnSeekEnd)(void *self);
  void (MilkTea_call *OnResume)(void *self);
  void (MilkTea_call *OnStop)(void *self, int64_t time);
  void (MilkTea_call *OnComplete)(void *self);
  void (MilkTea_call *OnReset)(void *self);
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Controller_Interface_t SoyMilk_Player_Controller_Interface_t;
#endif

struct SoyMilk_Player_Controller_t {
  void *self_;
  const SoyMilk_Player_Controller_Interface_t *interface_;
};

#ifndef __cplusplus
typedef struct SoyMilk_Player_Controller_t SoyMilk_Player_Controller_t;
#endif

struct SoyMilk_Player_t;

#ifndef __cplusplus
typedef struct SoyMilk_Player_t SoyMilk_Player_t;
#endif

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Create(SoyMilk_Player_t **self, TeaPot_TimerWorker_Weak_t *timer, SoyMilk_Player_Controller_t callback);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Destroy(SoyMilk_Player_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Prepare(SoyMilk_Player_t *self, const MilkPowder_Midi_t *midi);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Start(SoyMilk_Player_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Pause(SoyMilk_Player_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Seek(SoyMilk_Player_t *self, int64_t time);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Resume(SoyMilk_Player_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Stop(SoyMilk_Player_t *self);

MilkTea_api MilkTea_Exception_t MilkTea_call
SoyMilk_Player_Reset(SoyMilk_Player_t *self);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_H_
