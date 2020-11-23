#ifndef AIBOX_SDK_AI_SDK_H
#define AIBOX_SDK_AI_SDK_H

#include "ai_common.h"
#include "ai_sys.h"
#include "ai_vdec.h"
#include "ai_vpss.h"
#include "ai_venc.h"

#define AI_VDEC_MAX_WIDTH   1920
#define AI_VDEC_MAX_HEIGHT  1080
#define AI_VPSS_MAX_WIDTH   1920
#define AI_VPSS_MAX_HEIGHT  1080
#define AI_VENC_MAX_WIDTH   1920
#define AI_VENC_MAX_HEIGHT  1080

typedef enum AI_CHN_TYPE
{
    AI_CHN_TYPE_JPEGD,
    AI_CHN_TYPE_JPEGD_VPSS,
    AI_CHN_TYPE_JPEGD_JPEGE,
    AI_CHN_TYPE_JPEGD_VPSS_JPEGE,
    AI_CHN_TYPE_VPSS,
    AI_CHN_TYPE_VPSS_JPEGE,
    AI_CHN_TYPE_JPEGE,
    AI_CHN_TYPE_H264D_VPSS,
    AI_CHN_TYPE_H264D_VPSS_JPEGE,
} AI_CHN_TYPE_E;

typedef struct AI_IMAGE_INFO
{
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32Stride[3];
    void *pPhyAddr[3];
    void *pVirAddr[3];
} AI_IMAGE_INFO_S;

typedef struct AI_CHN_INFO
{
    AI_CHN_TYPE_E enType;
    HI_S32 s32Chn;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32VpssWidth;
    HI_U32 u32VpssHeight;
    HI_U32 u32VencWidth;
    HI_U32 u32VencHeight;
    VB_POOL hVbPool;
    HI_U8 *pStream;
    HI_U32 u32StreamLen;
    VIDEO_FRAME_INFO_S *pstVFrame;
    AI_IMAGE_INFO_S *pstImage;
} AI_CHN_INFO_S;

int ai_sdk_init(void);

int ai_sdk_exit(void);

int ai_sdk_create(AI_CHN_INFO_S *pstChnInfo);

int ai_sdk_destroy(AI_CHN_INFO_S *pstChnInfo);

int ai_sdk_send(AI_CHN_INFO_S *pstChnInfo);

int ai_sdk_get(AI_CHN_INFO_S *pstChnInfo);

int ai_sdk_release(AI_CHN_INFO_S *pstChnInfo);

#endif //AIBOX_SDK_AI_SDK_H
