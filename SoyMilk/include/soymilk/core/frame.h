#ifndef LIB_SOYMILK_CORE_FRAME_H_
#define LIB_SOYMILK_CORE_FRAME_H_

#include <milktea.h>
#include <milkpowder.h>

#ifdef __cplusplus
extern "C" {
#endif

struct SoyMilk_FrameBuffer_t;

#ifndef __cplusplus
typedef struct SoyMilk_FrameBuffer_t SoyMilk_FrameBuffer_t;
#endif

MilkTea_Function_t(SoyMilk_FrameBuffer_Consumer_t, void, size_t, uint16_t, const MilkPowder_Message_t *)

#ifndef __cplusplus
typedef struct SoyMilk_FrameBuffer_Consumer_t SoyMilk_FrameBuffer_Consumer_t;
#endif

MilkTea_api
SoyMilk_FrameBuffer_Clone(const SoyMilk_FrameBuffer_t *obj, SoyMilk_FrameBuffer_t **another);

MilkTea_api
SoyMilk_FrameBuffer_Destroy(SoyMilk_FrameBuffer_t *obj);

MilkTea_api
SoyMilk_FrameBuffer_GetTime(const SoyMilk_FrameBuffer_t *obj, int64_t *time);

MilkTea_api
SoyMilk_FrameBuffer_AllMessage(const SoyMilk_FrameBuffer_t *obj, SoyMilk_FrameBuffer_Consumer_t consumer);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifndef LIB_SOYMILK_CORE_FRAME_H_
