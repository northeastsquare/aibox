#ifndef AIBOX_SDK_AI_COMM_VDEC_H
#define AIBOX_SDK_AI_COMM_VDEC_H

#include "ai_common.h"

/*  ************************************************************************
    解码通道缓存信息
    ************************************************************************  */
typedef struct AI_COMM_VDEC_BUF
{
    HI_U32  u32PicBufSize;
    HI_U32  u32TmvBufSize;
    HI_BOOL bPicBufAlloc;
    HI_BOOL bTmvBufAlloc;
} AI_VDEC_BUF;

/*  ************************************************************************
    解码通道视频属性

    解码模式: I/IP/IPB
    数据位宽: 8/10/12/14/16
    ************************************************************************  */
typedef struct AI_COMM_VDEC_VIDEO_ATTR
{
    VIDEO_DEC_MODE_E    enDecMode;      // 解码模式
    DATA_BITWIDTH_E     enBitWidth;     // 数据位宽
    HI_U32              u32RefFrameNum; // 参考帧数
} AI_VDEC_VIDEO_ATTR;

/*  ************************************************************************
    解码通道图片属性

    像素格式: YVU_SEMIPLANAR_420
    ************************************************************************  */
typedef struct AI_COMM_VDEC_PICTURE_ATTR
{
    PIXEL_FORMAT_E  enPixelFormat;  // 像素格式
    HI_U32          u32Alpha;
} AI_VDEC_PICTURE_ATTR;

/*  ************************************************************************
    解码通道属性

    载荷类型: PT_H264/PT_H265/PT_MJPEG
    码流模式: STREAM/FRAME/COMPAT
    帧存计数:
        帧存计数(H.264/H.265)   >= 参考帧数 + 显示帧数 + 1
        帧存计数(Tmv)           >= 参考帧数 + 1
        帧存计数(JPEG/MJPEG)    >= 显示帧数 + 1
    视频属性: (H.264/H.265)
    图片属性: (JPEG/MJPEG)
    ************************************************************************  */
typedef struct AI_COMM_VDEC_ATTR
{
    PAYLOAD_TYPE_E              enType;             // 载荷类型
    VIDEO_MODE_E                enMode;             // 码流模式
    HI_U32                      u32Width;           // 图像宽度
    HI_U32                      u32Height;          // 图像高度
    HI_U32                      u32FrameBufCnt;     // 帧存计数
    HI_U32                      u32DisplayFrameNum; // 显示帧数
    union {
        AI_VDEC_VIDEO_ATTR      stAiVdecVideo;      // 视频属性
        AI_VDEC_PICTURE_ATTR    stAiVdecPicture;    // 图片属性
    };
    VB_POOL                     hVbPool;
} AI_VDEC_ATTR;

typedef struct ai_Stream_Arg
{
    PIXEL_FORMAT_E enType;    //数据类型
    HI_CHAR cFilePath[128];   //图像地址
    HI_U8 * cFileData;        //图像数据
    HI_S32 nBufflen;          //数据长度
    HI_S32 s32StreamMode;     //码流类型
    HI_S32 s32MilliSec;       //等待时长
    HI_U64  u64PtsInit;       //预留
    HI_U64  u64PtsIncrease;   //预留
}AI_STREAM_ARG;

typedef struct ai_Frame_Info
{
    HI_U32 uRealWidth;        //图像宽度
    HI_U32 uRealHeight;       //图像高度
    PIXEL_FORMAT_E enType;    //解析类型
}AI_FRAME_INFO;

/*  ************************************************************************
    SAMPLE_COMM_VDEC_GetPic           获取图像数据帧

    HI_S32          s32ChnId        收帧通道号
    HI_VOID         *pArgs          传参结构体
    unsigned char   *get_data       图像数据指针
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_GetPic(HI_S32 s32ChnId,HI_VOID *pArgs , unsigned char *get_data);

/*  ************************************************************************
    AI_CALC_BUF_SIZE                计算缓存池

    hiPIXEL_FORMAT_E   Type         图像类型
    HI_U32             Witdh        图像宽度
    ************************************************************************  */
HI_U32 AI_CALC_BUF_SIZE(PIXEL_FORMAT_E Type, HI_U32 Witdh);

/*  ************************************************************************
    AI_COMM_VDEC_ENBALEVPSS                设置VPSS开关

    HI_BOOL enable                         开关参数
    ************************************************************************  */
HI_VOID AI_COMM_VDEC_ENBALEVPSS(int channel, HI_BOOL enable);

/*  ************************************************************************
    AI_COMM_VDEC_AUTOSIZE                  设置是否自动记录宽高

    HI_BOOL auto_on                        开关参数
    ************************************************************************  */
HI_VOID AI_COMM_VDEC_AUTOSIZE(int channel, HI_BOOL auto_on);

/*  ************************************************************************
    AI_COMM_VDEC_Init               解码模块初始化
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_Init(HI_VOID);

/*  ************************************************************************
    AI_COMM_VDEC_CreateChannel      解码模块通道创建

    VDEC_CHN        VdecChn         解码模块通道号
    VDEC_CHN_ATTR_S *pstChnAttr     解码模块通道属性
    HI_BOOL         bStart          是否开始数据接收
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_CreateChannel(VDEC_CHN VdecChn, VDEC_CHN_ATTR_S *pstChnAttr, HI_BOOL bStart);

/*  ************************************************************************
    AI_COMM_VDEC_DestroyChannel     解码模块通道销毁

    VDEC_CHN        VdecChn         解码模块通道号
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_DestroyChannel(VDEC_CHN VdecChn);

/*  ************************************************************************
    AI_COMM_VDEC_SendStream        解码模块数据发送

    VDEC_CHN        VdecChn         解码模块通道号
    VDEC_STREAM_S   *pstStream      解码模块数据流
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_SendStream(VDEC_CHN VdecChn, VDEC_STREAM_S *pstStream);

/*  ************************************************************************
    AI_COMM_VDEC_GetFrame           解码模块数据帧获取

    VDEC_CHN        VdecChn         解码模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   解码模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_GetFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame);

/*  ************************************************************************
    AI_COMM_VDEC_ReleaseFrame       解码模块数据帧释放

    VDEC_CHN        VdecChn         解码模块通道号
    VIDEO_FRAME_INFO_S *pstVFrame   解码模块数据帧
    ************************************************************************  */
HI_S32 AI_COMM_VDEC_ReleaseFrame(VDEC_CHN VdecChn, VIDEO_FRAME_INFO_S *pstVFrame);

#endif //AIBOX_SDK_AI_COMM_VDEC_H
