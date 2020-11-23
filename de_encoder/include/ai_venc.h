#ifndef AIBOX_SDK_AI_VENC_H
#define AIBOX_SDK_AI_VENC_H

#include "ai_common.h"
#include "ai_comm_venc.h"

HI_S32 AI_VENC_Init(HI_VOID);

HI_S32 AI_VENC_CreateChannel(VENC_CHN VencChn, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32ImgWidth, HI_U32 u32ImgHeight);

HI_S32 AI_VENC_DestroyChannel(VENC_CHN VencChn);

HI_S32 AI_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 AI_VENC_GetStream(VENC_CHN VencChn, HI_U8 **ppStream, HI_U32 *u32Len);

#endif //AIBOX_SDK_AI_VENC_H
