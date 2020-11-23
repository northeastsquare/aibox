#ifndef AIBOX_SDK_AI_COMMON_H
#define AIBOX_SDK_AI_COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <hi_common.h>
#include <hi_buffer.h>
#include <hi_comm_sys.h>
#include <hi_comm_vb.h>
#include <hi_comm_venc.h>
#include <hi_comm_vdec.h>
#include <hi_comm_vpss.h>
#include <hi_comm_vgs.h>
#include <hi_comm_region.h>
#include <hi_comm_ive.h>
#include <hi_comm_vo.h>
#include <hi_comm_hdmi.h>
#include <mpi_sys.h>
#include <mpi_vb.h>
#include <mpi_venc.h>
#include <mpi_vdec.h>
#include <mpi_vpss.h>
#include <mpi_vgs.h>
#include <mpi_ive.h>
#include <mpi_region.h>
#include <mpi_vo.h>

#include <sys/ioctl.h>
#include "ai_comm_log.h"

typedef struct AI_IMAGE_ST
{
    HI_U32      width;          // 图像宽度
    HI_U32      height;         // 图像高度
    HI_U32      stride[3];      // 图像跨度
    void*       phy_addr[3];    // 物理地址
    void*       vir_addr[3];    // 虚拟地址
    HI_S32      channel;
    void*       frame_info;
    IVE_IMAGE_TYPE_E  enType;
} AI_IMAGE_S;

typedef struct rgn_overlay_para
{
    HI_U32 SX;                  //子区域起始位置X坐标
    HI_U32 SY;                  //子区域起始位置Y坐标
    HI_U32 WIDTH;               //区域宽度
    HI_U32 HEIGHT;              //区域高度
    HI_U32 BgAlpha;             //背景透明度
    HI_U32 FgAlpha;             //前景透明度
    HI_U32 BgColor;             //背景颜色
}RGN_OVERLAY_PARA;

#define AI_VENC_CHN_MAX_WIDTH   4096
#define AI_VENC_CHN_MAX_HEIGHT  4096

#define AI_VDEC_CHN_MAX_WIDTH   4096
#define AI_VDEC_CHN_MAX_HEIGHT  4096

#define AI_VPSS_GRP_MAX_WIDTH   4096
#define AI_VPSS_GRP_MAX_HEIGHT  4096


#endif //AIBOX_SDK_AI_COMMON_H
