#ifndef AIBOX_SDK_AI_VDEC_H
#define AIBOX_SDK_AI_VDEC_H

#include "ai_common.h"
#include "ai_comm_vdec.h"

HI_S32 AI_VDEC_Init(HI_VOID);

HI_S32 AI_VDEC_CreateChannel(VDEC_CHN VdecChn, PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height);

HI_S32 AI_VDEC_DestroyChannel(VDEC_CHN VdecChn);

HI_S32 AI_VDEC_SendStream(VDEC_CHN VdecChn, HI_U8 *pStream, HI_U32 u32Len);

HI_S32 AI_VDEC_GetFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 AI_VDEC_ReleaseFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame);

#endif //AIBOX_SDK_AI_VDEC_H
