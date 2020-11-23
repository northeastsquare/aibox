#include "ai_vpss.h"

HI_S32 AI_VPSS_Init(HI_VOID)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VPSS_Init();
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_Init: FAILURE\n");

        printf("AI_VPSS_Init: FAILURE\n");

        return HI_FAILURE;
    }

    printf("AI_VPSS_Init: SUCCESS\n");

    return HI_SUCCESS;
}
HI_S32 AI_VPSS_CreateChannel(VPSS_GRP VpssGrp, HI_U32 u32Width, HI_U32 u32Height ,HI_U32 u32ImgWidth, HI_U32 u32ImgHeight, VB_POOL *phVbPool)
{
    return AI_VPSS_CreateChannel_AP(VpssGrp, u32Width, u32Height ,u32ImgWidth, u32ImgHeight, phVbPool, ASPECT_RATIO_NONE);
}
HI_S32 AI_VPSS_CreateChannel_AP(VPSS_GRP VpssGrp, HI_U32 u32Width, HI_U32 u32Height ,HI_U32 u32ImgWidth, HI_U32 u32ImgHeight, VB_POOL *phVbPool, ASPECT_RATIO_E ap)
{
    int ai_func_log_index = AI_VPSS_LOG_INDEX;

    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    HI_BOOL bStart;
    VB_POOL_CONFIG_S stVbPoolCfg;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
    memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));

    stVpssGrpAttr.u32MaxW                       = u32Width;
    stVpssGrpAttr.u32MaxH                       = u32Height;
    stVpssGrpAttr.enPixelFormat                 = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.enDynamicRange                = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate   = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate   = -1;
    stVpssGrpAttr.bNrEn                         = HI_FALSE;

    stVpssChnAttr.enChnMode                     = VPSS_CHN_MODE_USER;
    stVpssChnAttr.u32Width                      = u32ImgWidth;
    stVpssChnAttr.u32Height                     = u32ImgHeight;
    stVpssChnAttr.enVideoFormat                 = VIDEO_FORMAT_LINEAR;
    stVpssChnAttr.enPixelFormat                 = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssChnAttr.enDynamicRange                = DYNAMIC_RANGE_SDR8;
    stVpssChnAttr.enCompressMode                = COMPRESS_MODE_NONE;
    stVpssChnAttr.stFrameRate.s32SrcFrameRate   = -1;
    stVpssChnAttr.stFrameRate.s32DstFrameRate   = -1;
    stVpssChnAttr.bMirror                       = HI_FALSE;
    stVpssChnAttr.bFlip                         = HI_FALSE;
    stVpssChnAttr.u32Depth                      = 2;
    stVpssChnAttr.stAspectRatio.enMode          =  ap;//ASPECT_RATIO_AUTO;

    bStart = HI_TRUE;

    s32Ret = AI_COMM_VPSS_CreateChannel(VpssGrp, 0, &stVpssGrpAttr, &stVpssChnAttr, bStart);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_CreateChannel: FAILURE\n");
        printf("AI_VPSS_CreateChannel (VpssGrp = %d): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }
    printf("create channel %d, finish\n", VpssGrp);
    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));

    stVbPoolCfg.u64BlkSize  = COMMON_GetPicBufferSize(
            u32ImgWidth,
            u32ImgHeight,
            PIXEL_FORMAT_YVU_SEMIPLANAR_420,
            DATA_BITWIDTH_8,
            COMPRESS_MODE_NONE,
            0);

    stVbPoolCfg.u32BlkCnt   = 16;
    stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
    printf("HI_MPI_VB_CreatePool start\n");
    *phVbPool = HI_MPI_VB_CreatePool(&stVbPoolCfg);
    printf("HI_MPI_VB_CreatePool finish\n");
    if (VB_INVALID_POOLID == *phVbPool) {
        printf("HI_MPI_VB_CreatePool: VB_INVALID_POOLID\n");

        s32Ret = AI_COMM_VPSS_DestroyChannel(VpssGrp, 0);
        if (HI_SUCCESS != s32Ret) {
            printf("AI_COMM_VPSS_DestroyChannel: FAILURE\n");
        }

        printf("AI_VPSS_CreateChannel (VpssGrp = %d): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }
    printf("HI_MPI_VPSS_AttachVbPool start\n");
    s32Ret = HI_MPI_VPSS_AttachVbPool(VpssGrp, 0, *phVbPool);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_AttachVbPool (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, 0, s32Ret);

        s32Ret = HI_MPI_VB_DestroyPool(*phVbPool);
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_VB_DestroyPool: FAILURE\n");
        }

        s32Ret = AI_COMM_VPSS_DestroyChannel(VpssGrp, 0);
        if (HI_SUCCESS != s32Ret) {
            printf("AI_COMM_VPSS_DestroyChannel: FAILURE\n");
        }

        printf("AI_VPSS_CreateChannel (VpssGrp = %d): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }

    printf("AI_VPSS_CreateChannel (VpssGrp = %d): SUCCESS\n", VpssGrp);

    return HI_SUCCESS;
}

HI_S32 AI_VPSS_DestroyChannel(VPSS_GRP VpssGrp, VB_POOL hVbPool)
{
    int ai_func_log_index = AI_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VPSS_DetachVbPool(VpssGrp, 0);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_DetachVbPool (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, 0, s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VB_DestroyPool(hVbPool);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VB_DestroyPool: FAILURE\n");

        return HI_FAILURE;
    }

    s32Ret = AI_COMM_VPSS_DestroyChannel(VpssGrp, 0);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_DestroyChannel: FAILURE\n");

        printf("AI_VPSS_DestroyChannel (VpssGrp = %#x): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }

    printf("AI_VPSS_DestroyChannel (VpssGrp = %d): SUCCESS\n", VpssGrp);

    return HI_SUCCESS;
}

HI_S32 AI_VPSS_SendFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VPSS_SendFrame(VpssGrp, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_SendFrame: FAILURE\n");

        printf("AI_VPSS_SendFrame (VpssGrp = %d): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }

//    printf("AI_VPSS_SendFrame (VpssGrp = %d): SUCCESS\n", VpssGrp);

    return HI_SUCCESS;
}

HI_S32 AI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    memset(pstVFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

    s32Ret = AI_COMM_VPSS_GetChnFrame(VpssGrp, 0, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_GetChnFrame: FAILURE\n");

        printf("AI_VPSS_GetChnFrame (VpssGrp = %d): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }

//    printf("AI_VPSS_GetChnFrame (VpssGrp = %d): SUCCESS\n", VpssGrp);

    return HI_SUCCESS;
}

HI_S32 AI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VPSS_ReleaseChnFrame(VpssGrp, 0, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_ReleaseChnFrame: FAILURE\n");

        printf("AI_VPSS_ReleaseChnFrame (VpssGrp = %d): FAILURE\n", VpssGrp);

        return HI_FAILURE;
    }

//    printf("AI_VPSS_ReleaseChnFrame (VpssGrp = %d): SUCCESS\n", VpssGrp);

    return HI_SUCCESS;
}
