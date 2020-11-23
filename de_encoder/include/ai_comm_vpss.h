#ifndef AIBOX_SDK_AI_COMM_VPSS_H
#define AIBOX_SDK_AI_COMM_VPSS_H

#include "ai_common.h"

/*  ************************************************************************
    AI_COMM_VPSS_Init               视频处理模块初始化
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_Init(HI_VOID);

/*  ************************************************************************
    AI_COMM_VPSS_CreateChannel      视频处理模块组创建

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VPSS_GRP_ATTR_S *pstVpssGrpAttr 视频处理模块组属性
    VPSS_CHN_ATTR_S *pastVpssChnAttr视频处理模块通道属性
    HI_BOOL         bStart          是否开启组和通道
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_CreateChannel(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
        VPSS_GRP_ATTR_S *pstVpssGrpAttr, VPSS_CHN_ATTR_S *pstVpssChnAttr, HI_BOOL bStart);

/*  ************************************************************************
    AI_COMM_VPSS_DestroyChannel     视频处理模块组销毁

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_DestroyChannel(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

/*  ************************************************************************
    AI_COMM_VPSS_SendFrame          视频处理模块数据帧发送

    VPSS_GRP        VpssGrp         视频处理模块组号
    VIDEO_FRAME_INFO_S *pstVFrame   视频处理模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_SendFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame);

/*  ************************************************************************
    AI_COMM_VPSS_GetChnFrame        视频处理模块通道数据帧获取

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   视频处理模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstVFrame);

/*  ************************************************************************
    AI_COMM_VPSS_ReleaseChnFrame    视频处理模块通道数据帧释放

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   视频处理模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstVFrame);

/*  ************************************************************************
    AI_COMM_VPSS_SetChnSize         视频处理模块通道图像大小设置

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    SIZE_S          stSize          视频处理模块通道图像大小
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_SetChnSize(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, SIZE_S stSize);

/*  ************************************************************************
    AI_COMM_VPSS_SetChnCrop         视频处理模块通道裁剪区域设置

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    RECT_S          stRect          视频处理模块通道裁剪区域
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, RECT_S stRect);


HI_VOID AI_COMM_VPSS_ENABLE(int channel, HI_BOOL enable);
#endif //AIBOX_SDK_AI_COMM_VPSS_H
