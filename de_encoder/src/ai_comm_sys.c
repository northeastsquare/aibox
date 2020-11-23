#include "ai_comm_sys.h"

/*  ************************************************************************
    AI_COMM_SYS_Init                系统控制模块初始化

    VB_CONFIG_S     *pstVbConfig    视频缓存模块配置
    ************************************************************************  */
HI_S32 AI_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    if (NULL == pstVbConfig) {
        printf("Error: VB_CONFIG_S is NULL!\n");

        return HI_FAILURE;
    }

    // 退出系统控制模块
    s32Ret = HI_MPI_SYS_Exit();
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_Exit: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    // 退出视频缓存模块
    s32Ret = HI_MPI_VB_Exit();
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VB_Exit: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    // 设置视频缓存模块
    s32Ret = HI_MPI_VB_SetConfig(pstVbConfig);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VB_SetConfig: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    // 初始化视频缓存模块
    s32Ret = HI_MPI_VB_Init();
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VB_Init: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    // 初始化系统控制模块
    s32Ret = HI_MPI_SYS_Init();
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_Init: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_SYS_Exit                系统控制模块退出
    ************************************************************************  */
HI_S32 AI_COMM_SYS_Exit(HI_VOID)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    // 退出系统控制模块
    s32Ret = HI_MPI_SYS_Exit();
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_Exit: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    // 退出视频缓存模块
    s32Ret = HI_MPI_VB_Exit();
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VB_Exit: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_Bind_VPSS          解码模块到视频处理模块通道绑定

    VDEC_CHN        VdecChn         解码模块通道号
    VPSS_GRP        VpssGrp         视频处理模块组号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    HI_S32 s32Ret = HI_FAILURE;

    stSrcChn.enModId    = HI_ID_VDEC;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = VdecChn;

    stDestChn.enModId   = HI_ID_VPSS;
    stDestChn.s32DevId  = VpssGrp;
    stDestChn.s32ChnId  = 0;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_Bind VDEC->VPSS: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_UnBind_VPSS        解码模块到视频处理模块通道解绑

    VDEC_CHN        VdecChn         解码模块通道号
    VPSS_GRP        VpssGrp         视频处理模块组号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    HI_S32 s32Ret = HI_FAILURE;

    stSrcChn.enModId    = HI_ID_VDEC;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = VdecChn;

    stDestChn.enModId   = HI_ID_VPSS;
    stDestChn.s32DevId  = VpssGrp;
    stDestChn.s32ChnId  = 0;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_UnBind VDEC->VPSS: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_Bind_VENC          视频处理模块到编码模块通道绑定

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    HI_S32 s32Ret = HI_FAILURE;

    stSrcChn.enModId    = HI_ID_VPSS;
    stSrcChn.s32DevId   = VpssGrp;
    stSrcChn.s32ChnId   = VpssChn;

    stDestChn.enModId   = HI_ID_VENC;
    stDestChn.s32DevId  = 0;
    stDestChn.s32ChnId  = VencChn;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_Bind VPSS->VENC: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_UnBind_VENC        视频处理模块到编码模块通道解绑

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    HI_S32 s32Ret = HI_FAILURE;

    stSrcChn.enModId    = HI_ID_VPSS;
    stSrcChn.s32DevId   = VpssGrp;
    stSrcChn.s32ChnId   = VpssChn;

    stDestChn.enModId   = HI_ID_VENC;
    stDestChn.s32DevId  = 0;
    stDestChn.s32ChnId  = VencChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_UnBind VPSS->VENC: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_Bind_VENC          解码模块到编码模块通道绑定

    VDEC_CHN        VdecChn         解码模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_Bind_VENC(VDEC_CHN VdecChn, VENC_CHN VencChn)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    HI_S32 s32Ret = HI_FAILURE;

    stSrcChn.enModId    = HI_ID_VDEC;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = VdecChn;

    stDestChn.enModId   = HI_ID_VENC;
    stDestChn.s32DevId  = 0;
    stDestChn.s32ChnId  = VencChn;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_Bind VDEC->VENC: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_UnBind_VENC        解码模块到编码模块通道解绑

    VDEC_CHN        VdecChn         解码模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_UnBind_VENC(VDEC_CHN VdecChn, VENC_CHN VencChn)
{
    int ai_func_log_index = AI_COMM_SYS_LOG_INDEX;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    HI_S32 s32Ret = HI_FAILURE;

    stSrcChn.enModId    = HI_ID_VDEC;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = VdecChn;

    stDestChn.enModId   = HI_ID_VENC;
    stDestChn.s32DevId  = 0;
    stDestChn.s32ChnId  = VencChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_SYS_UnBind VDEC->VENC: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
