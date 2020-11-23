#include "ai_comm_venc.h"

/*  ************************************************************************
    AI_COMM_VENC_Init               编码模块初始化
    ************************************************************************  */
HI_S32 AI_COMM_VENC_Init(HI_VOID)
{
    int ai_func_log_index = AI_COMM_VENC_LOG_INDEX;

    VENC_PARAM_MOD_S stModParam;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stModParam, 0, sizeof(VENC_PARAM_MOD_S));

    stModParam.enVencModType = MODTYPE_JPEGE;

    s32Ret = HI_MPI_VENC_GetModParam(&stModParam);
    if (0 != s32Ret) {
        printf("HI_MPI_VENC_GetModParam: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VENC_SetModParam(&stModParam);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_SetModParam: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VENC_CreateChannel      编码模块通道创建

    VENC_CHN        VencChn         编码模块通道号
    HI_BOOL         bStart          是否开始数据接收
    ************************************************************************  */
HI_S32 AI_COMM_VENC_CreateChannel(VENC_CHN VencChn, VENC_CHN_ATTR_S *pstChnAttr, HI_BOOL bStart)
{
    int ai_func_log_index = AI_COMM_VENC_LOG_INDEX;

    VENC_RECV_PIC_PARAM_S stRecvParam;
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VENC_CreateChn(VencChn, pstChnAttr);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_CreateChn (VencChn = %d): %#x\n", VencChn, s32Ret);

        return HI_FAILURE;
    }

    if (HI_TRUE == bStart) {
        memset(&stRecvParam, 0, sizeof(VENC_RECV_PIC_PARAM_S));

        stRecvParam.s32RecvPicNum = -1;

        s32Ret = HI_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_VENC_StartRecvFrame (VencChn = %d): %#x\n", VencChn, s32Ret);

            s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
            if (HI_SUCCESS != s32Ret) {
                printf("HI_MPI_VENC_DestroyChn (VencChn = %d): %#x\n", VencChn, s32Ret);
            }

            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VENC_DestroyChannel     编码模块通道销毁

    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VENC_DestroyChannel(VENC_CHN VencChn)
{
    int ai_func_log_index = AI_COMM_VENC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VENC_StopRecvFrame(VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_StopRecvFrame (VencChn = %d): %#x\n", VencChn, s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_DestroyChn (VencChn = %d): %#x\n", VencChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VENC_SendFrame          编码模块数据帧发送

    VENC_CHN        VencChn         编码模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   编码模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_COMM_VENC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VENC_SendFrame(VencChn, pstVFrame, -1);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_SendFrame (VencChn = %d): %#x\n", VencChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VENC_GetStream          编码模块数据流获取

    VENC_CHN        VencChn         编码模块通道号
    VENC_STREAM_S   *pstStream      编码模块数据流
    ************************************************************************  */
HI_S32 AI_COMM_VENC_GetStream(VENC_CHN VencChn, VENC_STREAM_S *pstStream)
{
    int ai_func_log_index = AI_COMM_VENC_LOG_INDEX;

    VENC_CHN_STATUS_S stStat;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stStat, 0, sizeof(VENC_CHN_STATUS_S));

    s32Ret = HI_MPI_VENC_QueryStatus(VencChn, &stStat);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_QueryStatus (VencChn = %d): %#x\n", VencChn, s32Ret);

        return HI_FAILURE;
    }

    if (0 == stStat.u32CurPacks) {
        printf("HI_MPI_VENC_QueryStatus (VencChn = %d): current frame is NULL\n", VencChn);

        return HI_FAILURE;
    }

    pstStream->pstPack = (VENC_PACK_S *) malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    if (NULL == pstStream->pstPack) {
        perror("malloc()");

        return HI_FAILURE;
    }
    pstStream->u32PackCount = stStat.u32CurPacks;

    s32Ret = HI_MPI_VENC_GetStream(VencChn, pstStream, -1);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_GetStream (VencChn = %d): %#x\n", VencChn, s32Ret);

        free(pstStream->pstPack);
        pstStream->pstPack = NULL;

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VENC_ReleaseStream      编码模块数据流释放

    VENC_CHN        VencChn         编码模块通道号
    VENC_STREAM_S   *pstStream      编码模块数据流
    ************************************************************************  */
HI_S32 AI_COMM_VENC_ReleaseStream(VENC_CHN VencChn, VENC_STREAM_S *pstStream)
{
    int ai_func_log_index = AI_COMM_VENC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, pstStream);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VENC_ReleaseStream (VencChn = %d): %#x\n", VencChn, s32Ret);

        free(pstStream->pstPack);
        pstStream->pstPack = NULL;

        return HI_FAILURE;
    }

    free(pstStream->pstPack);
    pstStream->pstPack = NULL;

    return HI_SUCCESS;
}
