#include "ai_venc.h"

HI_S32 AI_VENC_Init(HI_VOID)
{
    int ai_func_log_index = AI_VENC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VENC_Init();
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VENC_Init: FAILURE\n");

        printf("AI_VENC_Init: FAILURE\n");

        return HI_FAILURE;
    }

    printf("AI_VENC_Init: SUCCESS\n");

    return HI_SUCCESS;
}

HI_S32 AI_VENC_CreateChannel(VENC_CHN VencChn, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32ImgWidth, HI_U32 u32ImgHeight)
{
    int ai_func_log_index = AI_VENC_LOG_INDEX;

    VENC_CHN_ATTR_S stChnAttr;
    HI_BOOL bStart;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

    stChnAttr.stVencAttr.enType             = PT_JPEG;
    stChnAttr.stVencAttr.u32MaxPicWidth     = u32Width;
    stChnAttr.stVencAttr.u32MaxPicHeight    = u32Height;
    stChnAttr.stVencAttr.u32BufSize         = u32Width * u32Height * 2;
    stChnAttr.stVencAttr.u32Profile         = 0;
    stChnAttr.stVencAttr.bByFrame           = HI_TRUE;
    stChnAttr.stVencAttr.u32PicWidth        = u32ImgWidth;
    stChnAttr.stVencAttr.u32PicHeight       = u32ImgHeight;

    stChnAttr.stVencAttr.stAttrJpege.bSupportDCF                    = HI_FALSE;
    stChnAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum   = 0;
    stChnAttr.stVencAttr.stAttrJpege.enReceiveMode                  = VENC_PIC_RECEIVE_SINGLE;

    bStart = HI_TRUE;

    s32Ret = AI_COMM_VENC_CreateChannel(VencChn, &stChnAttr, bStart);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VENC_CreateChannel: FAILURE\n");

        printf("AI_VENC_CreateChannel (VencChn = %d): FAILURE\n", VencChn);

        return HI_FAILURE;
    }

    printf("AI_VENC_CreateChannel (VencChn = %d): SUCCESS\n", VencChn);

    return HI_SUCCESS;
}

HI_S32 AI_VENC_DestroyChannel(VENC_CHN VencChn)
{
    int ai_func_log_index = AI_VENC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VENC_DestroyChannel(VencChn);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VENC_DestroyChannel: FAILURE\n");

        printf("AI_VENC_DestroyChannel (VencChn = %d): FAILURE\n", VencChn);

        return HI_FAILURE;
    }

    printf("AI_VENC_DestroyChannel (VencChn = %d): SUCCESS\n", VencChn);

    return HI_SUCCESS;
}

HI_S32 AI_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_VENC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VENC_SendFrame(VencChn, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VENC_SendFrame: FAILURE\n");

        printf("AI_VENC_SendFrame (VencChn = %d): FAILURE\n", VencChn);

        return HI_FAILURE;
    }

//    printf("AI_VENC_SendFrame (VencChn = %d): SUCCESS\n", VencChn);

    return HI_SUCCESS;
}

HI_S32 AI_VENC_GetStream(VENC_CHN VencChn, HI_U8 **ppStream, HI_U32 *u32Len)
{
    int ai_func_log_index = AI_VENC_LOG_INDEX;

    VENC_STREAM_S stStream;
    HI_U8 *pTmp;
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stStream, 0, sizeof(VENC_STREAM_S));

    s32Ret = AI_COMM_VENC_GetStream(VencChn, &stStream);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VENC_GetStream: FAILURE\n");

        printf("AI_VENC_GetStream (VencChn = %d): FAILURE\n", VencChn);

        return HI_FAILURE;
    }

    *u32Len = 0;
    for (i = 0; i < stStream.u32PackCount; i++) {
        *u32Len += stStream.pstPack[i].u32Len - stStream.pstPack[i].u32Offset;
    }

    *ppStream = (HI_U8 *) malloc(*u32Len);
    if (NULL == *ppStream) {
        perror("malloc()");

        free(stStream.pstPack);
        stStream.pstPack = NULL;

        printf("AI_VENC_GetStream (VencChn = %d): FAILURE\n", VencChn);

        return HI_FAILURE;
    }

    pTmp = *ppStream;
    for (i = 0; i < stStream.u32PackCount; i++) {
        memcpy(pTmp, stStream.pstPack[i].pu8Addr + stStream.pstPack[i].u32Offset,
                stStream.pstPack[i].u32Len - stStream.pstPack[i].u32Offset);
        pTmp += stStream.pstPack[i].u32Len - stStream.pstPack[i].u32Offset;
    }

    s32Ret = AI_COMM_VENC_ReleaseStream(VencChn, &stStream);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VENC_ReleaseStream: FAILURE\n");

        printf("AI_VENC_GetStream (VencChn = %d): FAILURE\n", VencChn);

        return HI_FAILURE;
    }

//    printf("AI_VENC_GetStream (VencChn = %d): SUCCESS\n", VencChn);

    return HI_SUCCESS;
}
