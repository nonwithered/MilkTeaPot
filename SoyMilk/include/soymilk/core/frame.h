#ifndef LIB_SOYMILK_CORE_FRAME_H_
#define LIB_SOYMILK_CORE_FRAME_H_

#include <milktea.h>
#include <milkpowder.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyMilk_FrameEvent_t;

#ifndef __cplusplus
typedef struct SoyMilk_FrameEvent_t SoyMilk_FrameEvent_t;
#endif

MilkTea_api
SoyMilk_FrameEvent_GetNtrk(const SoyMilk_FrameEvent_t *obj, uint16_t *ntrk);

MilkTea_api
SoyMilk_FrameEvent_GetLen(const SoyMilk_FrameEvent_t *obj, size_t *len);

MilkTea_api
SoyMilk_FrameEvent_GetMsg(const SoyMilk_FrameEvent_t *obj, size_t idx, const MilkPowder_Message_t **msg);

struct SoyMilk_FrameBuffer_t;

#ifndef __cplusplus
typedef struct SoyMilk_FrameBuffer_t SoyMilk_FrameBuffer_t;
#endif

MilkTea_api
SoyMilk_FrameBuffer_Clone(SoyMilk_FrameBuffer_t **obj, const SoyMilk_FrameBuffer_t *another);

MilkTea_api
SoyMilk_FrameBuffer_Destroy(SoyMilk_FrameBuffer_t *obj);

MilkTea_api
SoyMilk_FrameBuffer_GetTime(const SoyMilk_FrameBuffer_t *obj, int64_t *time);

MilkTea_api
SoyMilk_FrameBuffer_GetLen(const SoyMilk_FrameBuffer_t *obj, size_t *len);

MilkTea_api
SoyMilk_FrameBuffer_GetItem(const SoyMilk_FrameBuffer_t *obj, size_t idx, const SoyMilk_FrameEvent_t **item);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_FRAME_H_
