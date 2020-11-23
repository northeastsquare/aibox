#ifndef AIBOX_SDK_AI_COMM_RGN_H
#define AIBOX_SDK_AI_COMM_RGN_H

#include "ai_common.h"
#include "loadbmp.h"




HI_S32 AI_RGN_ATTACH_BITMAP_TO_CHN(HI_S32 PcivChn, BITMAP_S *stBitmap, RGN_OVERLAY_PARA *param);

HI_S32 AI_RGN_Destory_Region(HI_S32 PcivChn);

HI_S32 AI_Load_Bmp(const char *filename, BITMAP_S *pstBitmap, HI_BOOL bFil, HI_U32 u16FilColor);


#endif