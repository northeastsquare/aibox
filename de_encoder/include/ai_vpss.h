#ifndef AIBOX_SDK_AI_VPSS_H
#define AIBOX_SDK_AI_VPSS_H

#include "ai_common.h"
#include "ai_comm_vpss.h"

HI_S32 AI_VPSS_Init(HI_VOID);

HI_S32 AI_VPSS_CreateChannel_AP(VPSS_GRP VpssGrp, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32ImgWidth, HI_U32 u32ImgHeight, VB_POOL *phVbPool, ASPECT_RATIO_E ap);

HI_S32 AI_VPSS_CreateChannel(VPSS_GRP VpssGrp, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32ImgWidth, HI_U32 u32ImgHeight, VB_POOL *phVbPool);

HI_S32 AI_VPSS_DestroyChannel(VPSS_GRP VpssGrp, VB_POOL hVbPool);

HI_S32 AI_VPSS_SendFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 AI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 AI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame);

#endif //AIBOX_SDK_AI_VPSS_H
