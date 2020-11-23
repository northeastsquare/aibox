#include "ai_comm_vpss.h"

/*  ************************************************************************
    AI_COMM_VPSS_Init               视频处理模块初始化
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_Init(HI_VOID)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    VPSS_MOD_PARAM_S stModParam;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stModParam, 0, sizeof(VPSS_MOD_PARAM_S));

    s32Ret = HI_MPI_VPSS_GetModParam(&stModParam);
    if (0 != s32Ret) {
        printf("HI_MPI_VPSS_GetModParam: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    stModParam.u32VpssVbSource = 2;

    s32Ret = HI_MPI_VPSS_SetModParam(&stModParam);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_SetModParam: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_CreateChannel      视频处理模块组创建

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VPSS_GRP_ATTR_S *pstVpssGrpAttr 视频处理模块组属性
    VPSS_CHN_ATTR_S *pastVpssChnAttr视频处理模块通道属性
    HI_BOOL         bStart          是否开启组和通道
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_CreateChannel(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
        VPSS_GRP_ATTR_S *pstVpssGrpAttr, VPSS_CHN_ATTR_S *pstVpssChnAttr, HI_BOOL bStart)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_CreateGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, pstVpssChnAttr);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_SetChnAttr (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_VPSS_DestroyGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);
        }

        return HI_FAILURE;
    }

    if (HI_TRUE == bStart) {
        s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_VPSS_EnableChn (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

            s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
            if (HI_SUCCESS != s32Ret) {
                printf("HI_MPI_VPSS_DestroyGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);
            }

            return HI_FAILURE;
        }

        s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_VPSS_StartGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);

            s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
            if (HI_SUCCESS != s32Ret) {
                printf("HI_MPI_VPSS_DestroyGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);
            }

            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_DestroyChannel     视频处理模块组销毁

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_DestroyChannel(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_StopGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_DisableChn (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_DestroyGrp (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_SendFrame          视频处理模块数据帧发送

    VPSS_GRP        VpssGrp         视频处理模块组号
    VIDEO_FRAME_INFO_S *pstVFrame   视频处理模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_SendFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VPSS_SendFrame(VpssGrp, 0, pstVFrame, -1);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_SendFrame (VpssGrp = %d): %#x\n", VpssGrp, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_GetChnFrame        视频处理模块通道数据帧获取

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   视频处理模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, pstVFrame, -1);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_GetChnFrame (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_ReleaseChnFrame    视频处理模块通道数据帧释放

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   视频处理模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_ReleaseChnFrame (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_SetChnSize         视频处理模块通道图像大小设置

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    SIZE_S          stSize          视频处理模块通道图像大小
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_SetChnSize(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, SIZE_S stSize)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    VPSS_CHN_ATTR_S stVpssChnAttr;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));

    s32Ret = HI_MPI_VPSS_GetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_GetChnAttr (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        return HI_FAILURE;
    }

    stVpssChnAttr.u32Width  = stSize.u32Width;
    stVpssChnAttr.u32Height = stSize.u32Height;

    s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_SetChnAttr (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VPSS_SetChnCrop         视频处理模块通道裁剪区域设置

    VPSS_GRP        VpssGrp         视频处理模块组号
    VPSS_CHN        VpssChn         视频处理模块通道号
    RECT_S          stRect          视频处理模块通道裁剪区域
    ************************************************************************  */
HI_S32 AI_COMM_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, RECT_S stRect)
{
    int ai_func_log_index = AI_COMM_VPSS_LOG_INDEX;

    VPSS_CROP_INFO_S stCropInfo;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stCropInfo, 0, sizeof(VPSS_CROP_INFO_S));

    stCropInfo.bEnable              = HI_TRUE;
    stCropInfo.enCropCoordinate     = VPSS_CROP_ABS_COOR;
    stCropInfo.stCropRect.s32X      = stRect.s32X;
    stCropInfo.stCropRect.s32Y      = stRect.s32Y;
    stCropInfo.stCropRect.u32Width  = stRect.u32Width;
    stCropInfo.stCropRect.u32Height = stRect.u32Height;
    if (stRect.u32Width <= 0 || stRect.u32Height <= 0)
    {
        stCropInfo.bEnable = HI_FALSE;
        stCropInfo.stCropRect.u32Width = stCropInfo.stCropRect.u32Height = 128;
    }

    s32Ret = HI_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &stCropInfo);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_SetChnCrop (VpssGrp = %d, VpssChn = %d): %#x\n", VpssGrp, VpssChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_VOID AI_COMM_VPSS_ENABLE(int channel, HI_BOOL enable)
{
    /*HI_S32 RES = 0;
    VPSS_TRIGGER[channel] = enable;
    memcpy(&vpss_para_resize[channel],resize_para,sizeof(VPSS_CHN_ATTR_S) );
    //memcpy(&vpss_para_crop[channel],crop_para,sizeof(VPSS_CROP_INFO_S));
    if (resize_para->u32Height > 0 && resize_para->u32Width > 0)
    {
        auto_size[channel] = HI_FALSE;
    }
    else  auto_size[channel] = HI_TRUE;
    if (crop_para->stCropRect.u32Width > 0 && crop_para->stCropRect.u32Height > 0 && crop_para->bEnable)
        {
            RES = HI_MPI_VPSS_SetChnCrop(channel, 0, crop_para);
        }
        else {
            crop_para->bEnable = HI_FALSE;
            crop_para->stCropRect.u32Width = crop_para->stCropRect.u32Height = crop_para->stCropRect.s32X = crop_para->stCropRect.s32Y = 16;
            RES = HI_MPI_VPSS_SetChnCrop(channel, 0, crop_para);
        }

        //HI_MPI_VPSS_SetChnRotation(channel,0,1);
        printf(" the crop res = %#x at chn %d, para %d, %d:%d:%d:%d\r\n", RES,channel,auto_size[channel],
        crop_para->stCropRect.u32Width,crop_para->stCropRect.u32Height,crop_para->stCropRect.s32X,crop_para->stCropRect.s32Y);*/
}
