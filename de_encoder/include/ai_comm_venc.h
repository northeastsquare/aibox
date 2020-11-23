#ifndef AIBOX_SDK_AI_COMM_VENC_H
#define AIBOX_SDK_AI_COMM_VENC_H

#include "ai_common.h"

/*  ************************************************************************
    AI_COMM_VENC_Init               编码模块初始化
    ************************************************************************  */
HI_S32 AI_COMM_VENC_Init(HI_VOID);

/*  ************************************************************************
    AI_COMM_VENC_CreateChannel      编码模块通道创建

    VENC_CHN        VencChn         编码模块通道号
    HI_BOOL         bStart          是否开始数据接收
    ************************************************************************  */
HI_S32 AI_COMM_VENC_CreateChannel(VENC_CHN VencChn, VENC_CHN_ATTR_S *pstChnAttr, HI_BOOL bStart);

/*  ************************************************************************
    AI_COMM_VENC_DestroyChannel     编码模块通道销毁

    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VENC_DestroyChannel(VENC_CHN VencChn);

/*  ************************************************************************
    AI_COMM_VENC_SendFrame          编码模块数据帧发送

    VENC_CHN        VencChn         编码模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   编码模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S *pstVFrame);

/*  ************************************************************************
    AI_COMM_VENC_GetStream          编码模块数据流获取

    VENC_CHN        VencChn         编码模块通道号
    VENC_STREAM_S   *pstStream      编码模块数据流
    ************************************************************************  */
HI_S32 AI_COMM_VENC_GetStream(VENC_CHN VencChn, VENC_STREAM_S *pstStream);

/*  ************************************************************************
    AI_COMM_VENC_ReleaseStream      编码模块数据流释放

    VENC_CHN        VencChn         编码模块通道号
    VENC_STREAM_S   *pstStream      编码模块数据流
    ************************************************************************  */
HI_S32 AI_COMM_VENC_ReleaseStream(VENC_CHN VencChn, VENC_STREAM_S *pstStream);

#endif //AIBOX_SDK_AI_COMM_VENC_H
