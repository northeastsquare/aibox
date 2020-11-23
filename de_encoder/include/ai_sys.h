#ifndef AIBOX_SDK_AI_SYS_H
#define AIBOX_SDK_AI_SYS_H

#include "ai_common.h"
#include "ai_comm_sys.h"

HI_S32 AI_SYS_Init(HI_VOID);

HI_S32 AI_SYS_Exit(HI_VOID);

HI_S32 AI_VDEC_Bind_VPSS(HI_S32 s32Channel);

HI_S32 AI_VDEC_UnBind_VPSS(HI_S32 s32Channel);

HI_S32 AI_VPSS_Bind_VENC(HI_S32 s32Channel);

HI_S32 AI_VPSS_UnBind_VENC(HI_S32 s32Channel);

HI_S32 AI_VDEC_Bind_VENC(HI_S32 s32Channel);

HI_S32 AI_VDEC_UnBind_VENC(HI_S32 s32Channel);

#endif //AIBOX_SDK_AI_SYS_H
