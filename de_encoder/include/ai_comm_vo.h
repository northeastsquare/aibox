
#ifndef AIBOX_SDK_AI_COMM_VO_H
#define AIBOX_SDK_AI_COMM_VO_H

#include "ai_common.h"
//#include "ai_comm_vo.h"
#include <hi_mipi_tx.h>

#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define TLV320_FILE "/dev/tlv320aic31"
#define COLOR_RGB_RED      0xFF0000
#define COLOR_RGB_GREEN    0x00FF00
#define COLOR_RGB_BLUE     0x0000FF
#define COLOR_RGB_BLACK    0x000000
#define COLOR_RGB_YELLOW   0xFFFF00
#define COLOR_RGB_CYN      0x00ffff
#define COLOR_RGB_WHITE    0xffffff

#define SAMPLE_VO_DEV_DHD0 0                  /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1 1                  /* VO's device HD1 */
#define SAMPLE_VO_DEV_UHD  SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD   SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VHD2 2
#define SAMPLE_VO_LAYER_PIP  SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 1
#define SAMPLE_AUDIO_INNER_AO_DEV 1
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 2

#define SAMPLE_AUDIO_PTNUMPERFRM   480

#define WDR_MAX_PIPE_NUM        4

#define SAMPLE_VO_DEF_VALUE (-1)
#define SAMPLE_VO_USE_DEFAULT_MIPI_TX 1

typedef enum AI_hiPIC_SIZE_E
{
    AI_PIC_CIF,
    AI_PIC_D1_PAL,    /* 720 * 576 */
    AI_PIC_D1_NTSC,   /* 720 * 480 */
    AI_PIC_720P,	   /* 1280 * 720  */
    AI_PIC_1080P,	   /* 1920 * 1080 */
    AI_PIC_1920x1088,
    AI_PIC_1920x1056,
    AI_PIC_1536x864,
    AI_PIC_1280x736,
    AI_PIC_1024X576,
    AI_PIC_2592x1520,
    AI_PIC_2592x1944,
    AI_PIC_3840x2160,
    AI_PIC_4096x2160,
    AI_PIC_3000x3000,
    AI_PIC_4000x3000,
    AI_PIC_7680x4320,
    AI_PIC_3840x8640,
    AI_PIC_BUTT
} AI_PIC_SIZE_E;

typedef enum AI_hiSAMPLE_VO_MODE_E
{
    AI_VO_MODE_1MUX  ,
    AI_VO_MODE_2MUX  ,
    AI_VO_MODE_4MUX  ,
    AI_VO_MODE_8MUX  ,
    AI_VO_MODE_9MUX  ,
    AI_VO_MODE_16MUX ,
    AI_VO_MODE_25MUX ,
    AI_VO_MODE_36MUX ,
    AI_VO_MODE_49MUX ,
    AI_VO_MODE_64MUX ,
    AI_VO_MODE_2X4   ,
    AI_VO_MODE_BUTT
} AI_VO_MODE_E;

typedef struct AI_hiSAMPLE_VO_CONFIG_S
{
    /* for device */
    VO_DEV                  VoDev;
    VO_INTF_TYPE_E          enVoIntfType;
    VO_INTF_SYNC_E          enIntfSync;
    AI_PIC_SIZE_E              enPicSize;
    HI_U32                  u32BgColor;

    /* for layer */
    PIXEL_FORMAT_E          enPixFormat;
    RECT_S                  stDispRect;
    SIZE_S                  stImageSize;
    VO_PART_MODE_E          enVoPartMode;

    HI_U32                  u32DisBufLen;
    DYNAMIC_RANGE_E         enDstDynamicRange;

    /* for chnnel */
    AI_VO_MODE_E        enVoMode;
} AI_VO_CONFIG_S;

typedef struct AI_vo_paras_def
{
    RECT_S Disp_Rect;
    int ChnNum;
    int VPSS_chn;
    VO_DEV Vodev;
    VO_INTF_TYPE_E VoIntfType;
}AI_VO_SET_ATTR;


AI_VO_MODE_E AI_GetVOMode(HI_U32 u32ChnNum);

HI_S32 AI_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 AI_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 AI_START_VO(AI_VO_SET_ATTR * attr);

HI_S32 AI_STOP_VO(AI_VO_SET_ATTR * attr);

HI_S32 AI_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S* pstPubAttr);

HI_S32 AI_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32* pu32W, HI_U32* pu32H, HI_U32* pu32Frm);

HI_S32 AI_COMM_VO_StopDev(VO_DEV VoDev);

HI_S32 AI_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S* pstLayerAttr);

HI_S32 AI_COMM_VO_StopLayer(VO_LAYER VoLayer);

HI_S32 AI_COMM_VO_StartChn(VO_LAYER VoLayer, AI_VO_MODE_E enMode);

HI_S32 AI_COMM_VO_StopChn(VO_LAYER VoLayer, AI_VO_MODE_E enMode);

HI_VOID AI_COMM_VO_HdmiConvertSync(VO_INTF_SYNC_E enIntfSync, HI_HDMI_VIDEO_FMT_E *penVideoFmt);

HI_S32 AI_COMM_VO_HdmiStop(HI_VOID);

HI_S32 AI_COMM_VO_HdmiStartByDyRg(VO_INTF_SYNC_E enIntfSync, DYNAMIC_RANGE_E enDyRg);

void AI_COMM_VO_StartMipiTx(VO_INTF_SYNC_E enVoIntfSync);


#endif //AIBOX_SDK_AI_COMM_VO_H