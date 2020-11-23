#ifndef AIBOX_SDK_AI_COMM_SYS_H
#define AIBOX_SDK_AI_COMM_SYS_H

#include "ai_common.h"

/*  ************************************************************************
    AI_COMM_SYS_Init                系统控制模块初始化

    VB_CONFIG_S     *pstVbConfig    视频缓存模块配置
    ************************************************************************  */
HI_S32 AI_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig);

/*  ************************************************************************
    AI_COMM_SYS_Exit                系统控制模块退出
    ************************************************************************  */
HI_S32 AI_COMM_SYS_Exit(HI_VOID);

/*  ************************************************************************
    AI_COMM_VDEC_Bind_VPSS          解码模块到视频处理模块通道绑定

    VDEC_CHN        VdecChn         解码模块通道号
    VPSS_GRP        VpssGrp         视频处理模块组号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);

/*  ************************************************************************
    AI_COMM_VDEC_UnBind_VPSS        解码模块到视频处理模块通道解绑

    VDEC_CHN        VdecChn         解码模块通道号
    VPSS_GRP        VpssGrp         视频处理模块组号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);

/*  ************************************************************************
    AI_COMM_VPSS_Bind_VENC          视频处理模块到编码模块通道绑定

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);

/*  ************************************************************************
    AI_COMM_VPSS_UnBind_VENC        视频处理模块到编码模块通道解绑

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);

/*  ************************************************************************
    AI_COMM_VDEC_Bind_VENC          解码模块到编码模块通道绑定

    VDEC_CHN        VdecChn         解码模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_Bind_VENC(VDEC_CHN VdecChn, VENC_CHN VencChn);

/*  ************************************************************************
    AI_COMM_VDEC_UnBind_VENC        解码模块到编码模块通道解绑

    VDEC_CHN        VdecChn         解码模块通道号
    VENC_CHN        VencChn         编码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_UnBind_VENC(VDEC_CHN VdecChn, VENC_CHN VencChn);

#endif //AIBOX_SDK_AI_COMM_SYS_H
