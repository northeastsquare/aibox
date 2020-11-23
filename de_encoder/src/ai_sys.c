#include "ai_sys.h"

HI_S32 AI_SYS_Init(HI_VOID)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VB_CONFIG_S stVbConfig;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    

    /////////////////
    stVbConfig.u32MaxPoolCnt              = 2;

    HI_U32 u32BlkSize = COMMON_GetPicBufferSize(1920*2, 1080*2, PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8,
     COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConfig.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConfig.astCommPool[0].u32BlkCnt   = 10;

    /////////////////////////////////
    s32Ret = AI_COMM_SYS_Init(&stVbConfig);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_SYS_Init: FAILURE\n");

        printf("AI_SYS_Init: FAILURE\n");

        return HI_FAILURE;
    }

    printf("AI_SYS_Init: SUCCESS\n");

    return HI_SUCCESS;
}

HI_S32 AI_SYS_Exit(HI_VOID)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_SYS_Exit();
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_SYS_Exit: FAILURE\n");

        printf("AI_SYS_Exit: FAILURE\n");

        return HI_FAILURE;
    }

    printf("AI_SYS_Exit: SUCCESS\n");

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_Bind_VPSS(HI_S32 s32Channel)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VDEC_CHN VdecChn;
    VPSS_GRP VpssGrp;

    HI_S32 s32Ret = HI_FAILURE;

    if (s32Channel < 0 || s32Channel > 64) {
        printf("Error: Channel is INVALID!\n");

        return HI_FAILURE;
    }

    VdecChn = s32Channel;
    VpssGrp = s32Channel;

    s32Ret = AI_COMM_VDEC_Bind_VPSS(VdecChn, VpssGrp);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_Bind_VPSS: FAILURE\n");

        printf("AI_VDEC_Bind_VPSS (Channel = %d): FAILURE\n", s32Channel);

        return HI_FAILURE;
    }

    printf("AI_VDEC_Bind_VPSS (Channel = %d): SUCCESS\n", s32Channel);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_UnBind_VPSS(HI_S32 s32Channel)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VDEC_CHN VdecChn;
    VPSS_GRP VpssGrp;

    HI_S32 s32Ret = HI_FAILURE;

    if (s32Channel < 0 || s32Channel > 64) {
        printf("Error: Channel is INVALID!\n");

        return HI_FAILURE;
    }

    VdecChn = s32Channel;
    VpssGrp = s32Channel;

    s32Ret = AI_COMM_VDEC_UnBind_VPSS(VdecChn, VpssGrp);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_UnBind_VPSS: FAILURE\n");

        printf("AI_VDEC_UnBind_VPSS (Channel = %d): FAILURE\n", s32Channel);

        return HI_FAILURE;
    }

    printf("AI_VDEC_UnBind_VPSS (Channel = %d): SUCCESS\n", s32Channel);

    return HI_SUCCESS;
}

HI_S32 AI_VPSS_Bind_VENC(HI_S32 s32Channel)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VPSS_GRP VpssGrp;
    VENC_CHN VencChn;

    HI_S32 s32Ret = HI_FAILURE;

    if (s32Channel < 0 || s32Channel > 64) {
        printf("Error: Channel is INVALID!\n");

        return HI_FAILURE;
    }

    VpssGrp = s32Channel;
    VencChn = s32Channel;

    s32Ret = AI_COMM_VPSS_Bind_VENC(VpssGrp, 0, VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_Bind_VENC: FAILURE\n");

        printf("AI_VPSS_Bind_VENC (Channel = %d): FAILURE\n", s32Channel);

        return HI_FAILURE;
    }

    printf("AI_VPSS_Bind_VENC (Channel = %d): SUCCESS\n", s32Channel);

    return HI_SUCCESS;
}

HI_S32 AI_VPSS_UnBind_VENC(HI_S32 s32Channel)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VPSS_GRP VpssGrp;
    VENC_CHN VencChn;

    HI_S32 s32Ret = HI_FAILURE;

    if (s32Channel < 0 || s32Channel > 64) {
        printf("Error: Channel is INVALID!\n");

        return HI_FAILURE;
    }

    VpssGrp = s32Channel;
    VencChn = s32Channel;

    s32Ret = AI_COMM_VPSS_UnBind_VENC(VpssGrp, 0, VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_UnBind_VENC: FAILURE\n");

        printf("AI_VPSS_UnBind_VENC (Channel = %d): FAILURE\n", s32Channel);

        return HI_FAILURE;
    }

    printf("AI_VPSS_UnBind_VENC (Channel = %d): SUCCESS\n", s32Channel);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_Bind_VENC(HI_S32 s32Channel)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VDEC_CHN VdecChn;
    VENC_CHN VencChn;

    HI_S32 s32Ret = HI_FAILURE;

    if (s32Channel < 0 || s32Channel > 64) {
        printf("Error: Channel is INVALID!\n");

        return HI_FAILURE;
    }

    VdecChn = s32Channel;
    VencChn = s32Channel;

    s32Ret = AI_COMM_VDEC_Bind_VENC(VdecChn, VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_Bind_VENC: FAILURE\n");

        printf("AI_VDEC_Bind_VENC (Channel = %d): FAILURE\n", s32Channel);

        return HI_FAILURE;
    }

    printf("AI_VDEC_Bind_VENC (Channel = %d): SUCCESS\n", s32Channel);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_UnBind_VENC(HI_S32 s32Channel)
{
    int ai_func_log_index = AI_SYS_LOG_INDEX;

    VDEC_CHN VdecChn;
    VENC_CHN VencChn;

    HI_S32 s32Ret = HI_FAILURE;

    if (s32Channel < 0 || s32Channel > 64) {
        printf("Error: Channel is INVALID!\n");

        return HI_FAILURE;
    }

    VdecChn = s32Channel;
    VencChn = s32Channel;

    s32Ret = AI_COMM_VDEC_UnBind_VENC(VdecChn, VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_UnBind_VENC: FAILURE\n");

        printf("AI_VDEC_UnBind_VENC (Channel = %d): FAILURE\n", s32Channel);

        return HI_FAILURE;
    }

    printf("AI_VDEC_UnBind_VENC (Channel = %d): SUCCESS\n", s32Channel);

    return HI_SUCCESS;
}
