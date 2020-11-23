#include "ai_comm_vdec.h"
#include "ai_comm_vgs.h"

VPSS_CROP_INFO_S vpss_para_crop[64];
VPSS_CHN_ATTR_S vpss_para_resize[64];
static HI_BOOL Auto_Size[64] = {HI_TRUE};
static HI_BOOL VPSS_Trigger[64] = {HI_FALSE};

unsigned char *data_ptr;
HI_U8 *pY_map = NULL;
HI_U8 *pC_map = NULL;
HI_U8 *pMemContent;
HI_U8 *pTmpBuff = HI_NULL;
HI_U8 *pUserPageAddr, *pTmp;

/*  ************************************************************************
    SAMPLE_COMM_VDEC_GetPic           获取图像数据帧

    HI_S32          s32ChnId        收帧通道号
    HI_VOID         *pArgs          传参结构体
    unsigned char   *get_data       图像数据指针
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_GetPic(HI_S32 s32ChnId, HI_VOID *pArgs, unsigned char *get_data)
{
    AI_FRAME_INFO *param = (AI_FRAME_INFO *)pArgs;
    VDEC_CHN_ATTR_S stAttr;
    VIDEO_FRAME_INFO_S stVFrame;
    VIDEO_FRAME_INFO_S _stVFrame;
    HI_S32 s32Ret = HI_MPI_VDEC_GetChnAttr(s32ChnId, &stAttr);
    HI_U32 u32WidthInBytes = 0;
    HI_U32 u32Size, s32Ysize;
    HI_U32 u32Stride;
    int good_len = 0;
    if (HI_SUCCESS != s32Ret)
    {
        printf("chn %d get chn attr fail for %#x!\n", s32ChnId, s32Ret);
        return HI_FAILURE;
    }

    if (PT_JPEG != stAttr.enType)
    {
        printf("chn %d enType %d do not suport save file!\n", s32ChnId, stAttr.enType);
        return HI_FAILURE;
    }
    s32Ret = HI_FALSE;
    int _out_count = 0;

    //s32Ret = HI_MPI_VPSS_SetChnCrop(s32ChnId, 0,&vpss_para);

    while (1)
    {
        
        s32Ret = HI_MPI_VDEC_GetFrame(s32ChnId, &stVFrame, -1);
        
        
        //s32Ret = HI_MPI_VPSS_SetChnCrop(s32ChnId, 0,&vpss_para);
        //s32Ret = HI_MPI_VPSS_GetChnFrame(s32ChnId, 0,&stVFrame, -1);
        //s32Ret = HI_MPI_VPSS_GetGrpFrame(s32ChnId, 0,&stVFrame);
        //VPSS_GRP _GRP;
        //if (stVFrame.stVFrame.u32Width > 0 && stVFrame.stVFrame.u32Height > 0 && stVFrame.stVFrame.enPixelFormat > 0 && s32Ret == HI_SUCCESS);    
        if (s32Ret == HI_SUCCESS)
            break;
        usleep(1);
    }

    if (VPSS_Trigger[s32ChnId])
    {
      /*  if (vpss_para_crop[s32ChnId].stCropRect.u32Width > 0 && vpss_para_crop[s32ChnId].stCropRect.u32Height > 0)
        {
            vpss_para_crop[s32ChnId].bEnable = HI_TRUE;
        }
        else vpss_para_crop[s32ChnId].bEnable = HI_FALSE;
        s32Ret = HI_MPI_VPSS_SetChnCrop(s32ChnId, 0, &vpss_para_crop[s32ChnId]);
        printf(" the crop res = %#x at chn %d\r\n", s32Ret,s32ChnId);*/


        if (Auto_Size[s32ChnId])
        {
            HI_S32 ori_w = stVFrame.stVFrame.u32Width;
            HI_S32 ori_h = stVFrame.stVFrame.u32Height;
            if (ori_w % 16 != 0)
                ori_w = ori_w / 2 * 2 + 2;
            if (ori_h % 2 != 0)
                ori_h += 1;
            memset(&vpss_para_resize[s32ChnId], 0, sizeof(VPSS_CHN_ATTR_S));

            s32Ret = HI_MPI_VPSS_GetChnAttr(s32ChnId, 0, &vpss_para_resize[s32ChnId]);
            if (ori_w != vpss_para_resize[s32ChnId].u32Width || ori_h != vpss_para_resize[s32ChnId].u32Height)
            {
                vpss_para_resize[s32ChnId].u32Width = ori_w;
                vpss_para_resize[s32ChnId].u32Height = ori_h;
                s32Ret = HI_MPI_VPSS_SetChnAttr(s32ChnId, 0, &vpss_para_resize[s32ChnId]);
                 printf("vdec & vpss set size %d %d = %#x\r\n", vpss_para_resize[s32ChnId].u32Width,vpss_para_resize[s32ChnId].u32Height,s32Ret);
            }
        }

        s32Ret = HI_MPI_VPSS_SendFrame(s32ChnId, 0, &stVFrame, -1);
        printf("send frame to vpss %#x\r\n", s32Ret);
        s32Ret = HI_MPI_VDEC_ReleaseFrame(s32ChnId, &stVFrame);
        printf("release VDEC frame%x\r\n", s32Ret);
        s32Ret = HI_MPI_VPSS_GetChnFrame(s32ChnId, 0, &stVFrame, -1);
        printf("get frame from vpss %#x, size = %d,%d\r\n", s32Ret, stVFrame.stVFrame.u32Width, stVFrame.stVFrame.u32Height);
    }

    good_len = stVFrame.stVFrame.u32Width;
    if (good_len % 16 != 0 && !VPSS_Trigger[s32ChnId])
        good_len = (good_len / 16 + 1) * 16;

    param->uRealHeight = stVFrame.stVFrame.u32Height;
    param->uRealWidth = good_len;
    param->enType = stVFrame.stVFrame.enPixelFormat;
  
    /*AI_VGS_Lines paraline;
     paraline.number = 2;
     paraline.line[0].Red = 255;
     paraline.line[0].Green = paraline.line[0].Blue = 0;
     paraline.line[0].thick = 6;
     paraline.line[0].start_pt.s32X = paraline.line[0].start_pt.s32Y = 0;
     paraline.line[0].end_pt.s32X = 288;
     paraline.line[0].end_pt.s32Y = 300;
     paraline.line[1].Blue = 255;
     paraline.line[1].Green = paraline.line[1].Red = 0;
     paraline.line[1].thick = 8;
     paraline.line[1].start_pt.s32X = paraline.line[1].start_pt.s32Y = 100;
     paraline.line[1].end_pt.s32X = 108;
     paraline.line[1].end_pt.s32Y = 300;
    AI_VGS_DRAW_LINES(&stVFrame,&paraline);

    AI_VGS_Quadrangles paraquad;
    paraquad.number = 2;
    paraquad.quad[0].SOLID = HI_FALSE;
    paraquad.quad[0].Blue = paraquad.quad[0].Red = paraquad.quad[0].Green = 255;
    paraquad.quad[0].thick = 2;
    paraquad.quad[0].the_point[0].s32X = paraquad.quad[0].the_point[0].s32Y = 100;
    paraquad.quad[0].the_point[3].s32X = paraquad.quad[0].the_point[3].s32Y = 400;
    paraquad.quad[0].the_point[1].s32X = 400;
    paraquad.quad[0].the_point[1].s32Y = 100;
    paraquad.quad[0].the_point[2].s32X = 100;
    paraquad.quad[0].the_point[2].s32Y = 400;
    paraquad.quad[1].SOLID = HI_TRUE;
    paraquad.quad[1].Blue = paraquad.quad[1].Red = paraquad.quad[1].Green = 162;
    paraquad.quad[1].thick = 4;
    paraquad.quad[1].the_point[0].s32X = paraquad.quad[1].the_point[0].s32Y = 400;
    paraquad.quad[1].the_point[3].s32X = paraquad.quad[1].the_point[3].s32Y = 800;
    paraquad.quad[1].the_point[1].s32X = 800;
    paraquad.quad[1].the_point[1].s32Y = 400;
    paraquad.quad[1].the_point[2].s32X = 400;
    paraquad.quad[1].the_point[2].s32Y = 800;
    AI_VGS_DRAW_QUADS(&stVFrame,&paraquad);
    ROTATION_E r_para = ROTATION_180;
    AI_VGS_DRAW_ROTATE(&stVFrame, &r_para);*/
    if ((PIXEL_FORMAT_YVU_SEMIPLANAR_420 != stVFrame.stVFrame.enPixelFormat) && (PIXEL_FORMAT_YUV_400 != stVFrame.stVFrame.enPixelFormat))
    {
        u32WidthInBytes = AI_CALC_BUF_SIZE(stVFrame.stVFrame.enPixelFormat, stVFrame.stVFrame.u32Width);
        u32Stride = ALIGN_UP(u32WidthInBytes, 16);
        u32Size = u32Stride * ALIGN_UP(stVFrame.stVFrame.u32Height, 16);
        pUserPageAddr = (HI_U8 *)HI_MPI_SYS_Mmap(stVFrame.stVFrame.u64PhyAddr[0], u32Size);
       
        if (HI_NULL == pUserPageAddr)
        {
            printf("%s %d:HI_MPI_SYS_Mmap fail!!! u32Size=%d\n", __func__, __LINE__, u32Size);
        }
        else
        {

            pTmp = pUserPageAddr;
            //unsigned int len = u32WidthInBytes;
            data_ptr = get_data;

            for (int i = 0; i < param->uRealHeight; i++, pTmp += u32Stride)
            {
                memcpy(data_ptr, pTmp, param->uRealWidth * 3);
                data_ptr += param->uRealWidth * 3;
            }
            HI_MPI_SYS_Munmap(pUserPageAddr, u32Size);
            pUserPageAddr = HI_NULL;
        }
    }
    else if ((stVFrame.stVFrame.enCompressMode == COMPRESS_MODE_NONE) &&
             (stVFrame.stVFrame.enVideoFormat == VIDEO_FORMAT_LINEAR))
    {
        HI_U32 u32UvHeight;
      
        if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == stVFrame.stVFrame.enPixelFormat)
        {
            s32Ysize = (stVFrame.stVFrame.u64PhyAddr[1] - stVFrame.stVFrame.u64PhyAddr[0]);
            u32Size = s32Ysize * 3 / 2;
            u32UvHeight = stVFrame.stVFrame.u32Height / 2;
        }
        else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == stVFrame.stVFrame.enPixelFormat)
        {
            s32Ysize = (stVFrame.stVFrame.u64PhyAddr[1] - stVFrame.stVFrame.u64PhyAddr[0]);
            u32Size = s32Ysize * 2;
            u32UvHeight = stVFrame.stVFrame.u32Height;
        }
        else if (PIXEL_FORMAT_YUV_400 == stVFrame.stVFrame.enPixelFormat)
        {
            u32Size = s32Ysize = (stVFrame.stVFrame.u32Stride[0]) * ALIGN_UP(stVFrame.stVFrame.u32Height, 16);
            u32UvHeight = 0;
        }
        else
        {
            printf("%s %d: This YUV format is not support!\n", __func__, __LINE__);
            //HI_MPI_VDEC_ReleaseFrame(s32ChnId, &stVFrame);
            if (VPSS_Trigger[s32ChnId])
                HI_MPI_VPSS_ReleaseChnFrame(s32ChnId,0, &stVFrame);
            else HI_MPI_VDEC_ReleaseFrame(s32ChnId, &stVFrame);
             return -1;
        }
        HI_U64 phy_addr = stVFrame.stVFrame.u64PhyAddr[0];
      
        pY_map = (HI_U8*) HI_MPI_SYS_Mmap(phy_addr, u32Size);
        if (HI_NULL == pY_map)
        {
            printf("HI_MPI_SYS_Mmap for pY_map fail!!\n");
        }
        else
        {
            data_ptr = get_data;
          
            //memcpy(data_ptr,pY_map,u32Size);

            for (int h = 0; h < stVFrame.stVFrame.u32Height; h++)
            {
                pMemContent = pY_map + h * stVFrame.stVFrame.u32Stride[0];
                //fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
                memcpy(data_ptr, pMemContent, param->uRealWidth);
                data_ptr += param->uRealWidth;
            }
            if(PIXEL_FORMAT_YUV_400 != stVFrame.stVFrame.enPixelFormat)
            {
                pC_map = pY_map + s32Ysize;
                for (int h = 0; h < u32UvHeight; h++)
                {
                    pMemContent = pC_map + h * stVFrame.stVFrame.u32Stride[1];
                    memcpy(data_ptr, pMemContent, param->uRealWidth);
                    data_ptr += param->uRealWidth;
                }
            }
            HI_MPI_SYS_Munmap(pY_map, u32Size);
            pY_map = HI_NULL;
        }
    }
    else
    {
        printf("chn %d enPixelFormat %d do not suport!\n", s32ChnId, stVFrame.stVFrame.enPixelFormat);
    }
    //s32Ret = HI_MPI_VO_SendFrame(0,0,&stVFrame,-1);
    //printf("send frame to vo %#x \r\n",s32Ret);
    if (VPSS_Trigger[s32ChnId])
        HI_MPI_VPSS_ReleaseChnFrame(s32ChnId,0, &stVFrame);
    else HI_MPI_VDEC_ReleaseFrame(s32ChnId, &stVFrame);
    printf("release frame%x\r\n",s32Ret);
    return HI_SUCCESS;
}
/*  ************************************************************************
    AI_CALC_BUF_SIZE                计算缓存池

    hiPIXEL_FORMAT_E   Type         图像类型
    HI_U32             Witdh        图像宽度
    ************************************************************************  */

HI_U32 AI_CALC_BUF_SIZE(PIXEL_FORMAT_E Type, HI_U32 Witdh)
{
    if ((PIXEL_FORMAT_ARGB_8888 == Type) || (PIXEL_FORMAT_ABGR_8888 == Type))
    {
        return 4 * Witdh;
    }
    else if( (PIXEL_FORMAT_ARGB_1555 == Type) || (PIXEL_FORMAT_ABGR_1555 == Type))
    {
        return 2*Witdh;
    }
    else if( (PIXEL_FORMAT_RGB_888 == Type) || (PIXEL_FORMAT_BGR_888 == Type))
    {
        return 3*Witdh;
    }
    else if( (PIXEL_FORMAT_RGB_565 == Type) || (PIXEL_FORMAT_BGR_565 == Type))
    {
        return 2*Witdh;
    }
    return 3*Witdh;
}
/*  ************************************************************************
    AI_COMM_VDEC_ENBALEVPSS                设置VPSS开关

    HI_BOOL enable                         开关参数
    ************************************************************************  */
HI_VOID AI_COMM_VDEC_ENBALEVPSS(int channel, HI_BOOL enable)
{
    VPSS_Trigger[channel] = enable;
}

/*  ************************************************************************
    AI_COMM_VDEC_AUTOSIZE                  设置是否自动记录宽高

    HI_BOOL auto_on                        开关参数
    ************************************************************************  */
HI_VOID AI_COMM_VDEC_AUTOSIZE(int channel, HI_BOOL auto_on)
{
    Auto_Size[channel] = auto_on;
}

/*  ************************************************************************
    AI_COMM_VDEC_Init               解码模块初始化
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_Init(HI_VOID)
{
    int ai_func_log_index = AI_COMM_VDEC_LOG_INDEX;

    VDEC_MOD_PARAM_S stModParam;
    HI_S32 s32Ret = HI_FAILURE;

    memset(&stModParam, 0, sizeof(VDEC_MOD_PARAM_S));

    s32Ret = HI_MPI_VDEC_GetModParam(&stModParam);
    if (0 != s32Ret) {
        printf("HI_MPI_VDEC_GetModParam: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    stModParam.enVdecVBSource = VB_SOURCE_PRIVATE;

    s32Ret = HI_MPI_VDEC_SetModParam(&stModParam);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_SetModParam: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_CreateChannel      解码模块通道创建

    VDEC_CHN        VdecChn         解码模块通道号
    VDEC_CHN_ATTR_S *pstChnAttr     解码模块通道属性
    HI_BOOL         bStart          是否开始数据接收
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_CreateChannel(VDEC_CHN VdecChn, VDEC_CHN_ATTR_S *pstChnAttr, HI_BOOL bStart)
{
    int ai_func_log_index = AI_COMM_VDEC_LOG_INDEX;

    VDEC_CHN_PARAM_S stChnParam;
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VDEC_CreateChn(VdecChn, pstChnAttr);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_CreateChn (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    memset(&stChnParam, 0, sizeof(VDEC_CHN_PARAM_S));

    s32Ret = HI_MPI_VDEC_GetChnParam(VdecChn, &stChnParam);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_GetChnParam (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    if (PT_JPEG == stChnParam.enType) {
        stChnParam.u32DisplayFrameNum               = 2;

        stChnParam.stVdecPictureParam.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        stChnParam.stVdecPictureParam.u32Alpha      = 255;

    } else if (PT_H264 == stChnParam.enType) {
        stChnParam.u32DisplayFrameNum               = 2;

        stChnParam.stVdecVideoParam.enDecMode       = VIDEO_DEC_MODE_IPB;
        stChnParam.stVdecVideoParam.enOutputOrder   = VIDEO_OUTPUT_ORDER_DISP;
        stChnParam.stVdecVideoParam.enCompressMode  = COMPRESS_MODE_TILE;
        stChnParam.stVdecVideoParam.enVideoFormat   = VIDEO_FORMAT_TILE_64x16;

    } else if (PT_H265 == stChnParam.enType) {
        stChnParam.u32DisplayFrameNum               = 2;

        stChnParam.stVdecVideoParam.enDecMode       = VIDEO_DEC_MODE_IPB;
        stChnParam.stVdecVideoParam.enOutputOrder   = VIDEO_OUTPUT_ORDER_DISP;
        stChnParam.stVdecVideoParam.enCompressMode  = COMPRESS_MODE_TILE;
        stChnParam.stVdecVideoParam.enVideoFormat   = VIDEO_FORMAT_TILE_64x16;

    }

    s32Ret = HI_MPI_VDEC_SetChnParam(VdecChn, &stChnParam);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_SetChnParam (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    if (HI_TRUE == bStart) {
        s32Ret = HI_MPI_VDEC_StartRecvStream(VdecChn);
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_VDEC_StartRecvStream (VdecChn = %d): %#x\n", VdecChn, s32Ret);

            s32Ret = HI_MPI_VDEC_DestroyChn(VdecChn);
            if (HI_SUCCESS != s32Ret) {
                printf("HI_MPI_VDEC_DestroyChn (VdecChn = %d): %#x\n", VdecChn, s32Ret);
            }

            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_DestroyChannel     解码模块通道销毁

    VDEC_CHN        VdecChn         解码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_DestroyChannel(VDEC_CHN VdecChn)
{
    int ai_func_log_index = AI_COMM_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VDEC_StopRecvStream(VdecChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_StopRecvStream (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VDEC_DestroyChn(VdecChn);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_DestroyChn (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_SendStream        解码模块数据流发送

    VDEC_CHN        VdecChn         解码模块通道号
    VDEC_STREAM_S   *pstStream      解码模块数据流
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_SendStream(VDEC_CHN VdecChn, VDEC_STREAM_S *pstStream)
{
    int ai_func_log_index = AI_COMM_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VDEC_SendStream(VdecChn, pstStream, -1);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_SendStream (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_GetFrame           解码模块数据帧获取

    VDEC_CHN        VdecChn         解码模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   解码模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_GetFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_COMM_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VDEC_GetFrame(VdecChn, pstVFrame, -1);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_GetFrame (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*  ************************************************************************
    AI_COMM_VDEC_ReleaseFrame       解码模块数据帧释放

    VDEC_CHN        VdecChn         解码模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   解码模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_ReleaseFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame)
{
    int ai_func_log_index = AI_COMM_VDEC_LOG_INDEX;

    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_MPI_VDEC_ReleaseFrame(VdecChn, pstVFrame);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VDEC_ReleaseFrame (VdecChn = %d): %#x\n", VdecChn, s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
