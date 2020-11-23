#include "ai_vdec.h"

HI_S32 AI_VDEC_Init(HI_VOID)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VDEC_Init();
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_Init: FAILURE\n");

        printf("AI_VDEC_Init: FAILURE\n");

        return HI_FAILURE;
    }

    printf("AI_VDEC_Init: SUCCESS\n");

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_CreateChannel(VDEC_CHN VdecChn, PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    VDEC_CHN_ATTR_S stChnAttr;
    HI_BOOL bStart;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stChnAttr, 0, sizeof(VDEC_CHN_ATTR_S));

    if (PT_JPEG == enType) {
        stChnAttr.enType            = PT_JPEG;
        stChnAttr.enMode            = VIDEO_MODE_FRAME;
        stChnAttr.u32PicWidth       = u32Width;
        stChnAttr.u32PicHeight      = u32Height;
        stChnAttr.u32StreamBufSize  = u32Width * u32Height;
        stChnAttr.u32FrameBufCnt    = 3;

        stChnAttr.u32FrameBufSize   = VDEC_GetPicBufferSize(
                PT_JPEG,
                u32Width,
                u32Height,
                PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                DATA_BITWIDTH_8,
                0);

    } else if (PT_H264 == enType) {
        stChnAttr.enType            = PT_H264;
        stChnAttr.enMode            = VIDEO_MODE_FRAME;
        stChnAttr.u32PicWidth       = u32Width;
        stChnAttr.u32PicHeight      = u32Height;
        stChnAttr.u32StreamBufSize  = u32Width * u32Height;
        stChnAttr.u32FrameBufCnt    = 6+5;

        stChnAttr.u32FrameBufSize   = VDEC_GetPicBufferSize(
                PT_H264,
                u32Width,
                u32Height,
                PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                DATA_BITWIDTH_8,
                0);

        stChnAttr.stVdecVideoAttr.u32RefFrameNum        = 3+5;
        stChnAttr.stVdecVideoAttr.bTemporalMvpEnable    = 1;

        stChnAttr.stVdecVideoAttr.u32TmvBufSize         = VDEC_GetTmvBufferSize(
                PT_H264,
                u32Width,
                u32Height);

    } else if (PT_H265 == enType) {
        stChnAttr.enType            = PT_H265;
        stChnAttr.enMode            = VIDEO_MODE_FRAME;
        stChnAttr.u32PicWidth       = u32Width;
        stChnAttr.u32PicHeight      = u32Height;
        stChnAttr.u32StreamBufSize  = u32Width * u32Height;
        stChnAttr.u32FrameBufCnt    = 6;

        stChnAttr.u32FrameBufSize   = VDEC_GetPicBufferSize(
                PT_H265,
                u32Width,
                u32Height,
                PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                DATA_BITWIDTH_8,
                0);

        stChnAttr.stVdecVideoAttr.u32RefFrameNum        = 3;
        stChnAttr.stVdecVideoAttr.bTemporalMvpEnable    = 1;

        stChnAttr.stVdecVideoAttr.u32TmvBufSize         = VDEC_GetTmvBufferSize(
                PT_H265,
                u32Width,
                u32Height);

    }

    bStart = HI_TRUE;

    s32Ret = AI_COMM_VDEC_CreateChannel(VdecChn, &stChnAttr, bStart);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_CreateChannel: FAILURE\n");

        printf("AI_VDEC_CreateChannel (VdecChn = %d): FAILURE\n", VdecChn);

        return HI_FAILURE;
    }

    printf("AI_VDEC_CreateChannel (VdecChn = %d): SUCCESS\n", VdecChn);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_DestroyChannel(VDEC_CHN VdecChn)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VDEC_DestroyChannel(VdecChn);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_DestroyChannel: FAILURE\n");

        printf("AI_VDEC_DestroyChannel (VdecChn = %d): FAILURE\n", VdecChn);

        return HI_FAILURE;
    }

    printf("AI_VDEC_DestroyChannel (VdecChn = %d): SUCCESS\n", VdecChn);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_SendStream(VDEC_CHN VdecChn, HI_U8 *pStream, HI_U32 u32Len)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    VDEC_STREAM_S stStream;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stStream, 0, sizeof(VDEC_STREAM_S));

    stStream.u32Len         = u32Len;
    stStream.u64PTS         = 0;
    stStream.bEndOfFrame    = HI_TRUE;
    stStream.bEndOfStream   = HI_TRUE;
    stStream.bDisplay       = HI_TRUE;
    stStream.pu8Addr        = pStream;

    s32Ret = AI_COMM_VDEC_SendStream(VdecChn, &stStream);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_SendStream: FAILURE\n");

        printf("AI_VDEC_SendStream (VdecChn = %d): FAILURE\n", VdecChn);

        return HI_FAILURE;
    }

//    printf("AI_VDEC_SendStream (VdecChn = %d): SUCCESS\n", VdecChn);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_GetFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    memset(pstVFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

    s32Ret = AI_COMM_VDEC_GetFrame(VdecChn, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_GetFrame: FAILURE\n");

        printf("AI_VDEC_GetFrame (VdecChn = %d): FAILURE\n", VdecChn);

        return HI_FAILURE;
    }

//    printf("AI_VDEC_GetFrame (VdecChn = %d): SUCCESS\n", VdecChn);

    return HI_SUCCESS;
}

HI_S32 AI_VDEC_ReleaseFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = AI_COMM_VDEC_ReleaseFrame(VdecChn, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_ReleaseFrame: FAILURE\n");

        printf("AI_VDEC_ReleaseFrame (VdecChn = %d): FAILURE\n", VdecChn);

        return HI_FAILURE;
    }

//    printf("AI_VDEC_ReleaseFrame (VdecChn = %d): SUCCESS\n", VdecChn);

    return HI_SUCCESS;
}
