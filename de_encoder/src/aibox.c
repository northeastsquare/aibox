#include "aibox.h"
#include "ai_sdk.h"
#include "ai_util.h"

unsigned char * transfer_buff[128] = {NULL}; //图像数据中转BUFF
unsigned char * ret_ptr;                      //指向IMAGE结构体的临时指针

const int ALIGN_W_MIN = 2;
const int ALIGN_H_MIN = 2;
AI_VDEC_ATTR astAiVdec[64];
VPSS_GRP_ATTR_S stVpssGrpAttr;
VPSS_CHN_ATTR_S stVpssChnAttr;
VIDEO_FRAME_INFO_S astVFrame[32];

AI_CHN_INFO_S astChnInfo[64] = {0};

static int ai_vdec_init(void)
{
    HI_S32 i = 0;

    memset(astAiVdec, 0, sizeof(AI_VDEC_ATTR) * 64);

    for (i = 0; i < 16; i++) {
        astAiVdec[i].enType                         = PT_H264;
        astAiVdec[i].enMode                         = VIDEO_MODE_FRAME;
        astAiVdec[i].u32Width                       = VIDEO_MAX_W;
        astAiVdec[i].u32Height                      = VIDEO_MAX_H;
        astAiVdec[i].u32FrameBufCnt                 = 6;
        astAiVdec[i].u32DisplayFrameNum             = 2;

        astAiVdec[i].stAiVdecVideo.enDecMode        = VIDEO_DEC_MODE_IPB;
        astAiVdec[i].stAiVdecVideo.enBitWidth       = DATA_BITWIDTH_8;
        astAiVdec[i].stAiVdecVideo.u32RefFrameNum   = 3;
    }

    for (i = 16; i < 32; i++) {
        astAiVdec[i].enType                         = PT_H265;
        astAiVdec[i].enMode                         = VIDEO_MODE_FRAME;
        astAiVdec[i].u32Width                       = VIDEO_MAX_W;
        astAiVdec[i].u32Height                      = VIDEO_MAX_H;
        astAiVdec[i].u32FrameBufCnt                 = 6;
        astAiVdec[i].u32DisplayFrameNum             = 2;

        astAiVdec[i].stAiVdecVideo.enDecMode        = VIDEO_DEC_MODE_IPB;
        astAiVdec[i].stAiVdecVideo.enBitWidth       = DATA_BITWIDTH_8;
        astAiVdec[i].stAiVdecVideo.u32RefFrameNum   = 3;
    }

    for (i = 32; i < 64; i++) {
        astAiVdec[i].enType                         = PT_JPEG;
        astAiVdec[i].enMode                         = VIDEO_MODE_FRAME;
        astAiVdec[i].u32Width                       = IMAGE_MAX_W;
        astAiVdec[i].u32Height                      = IMAGE_MAX_H;
        astAiVdec[i].u32FrameBufCnt                 = 3;
        astAiVdec[i].u32DisplayFrameNum             = 2;

        astAiVdec[i].stAiVdecPicture.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        astAiVdec[i].stAiVdecPicture.u32Alpha       = 255;
    }

    return HI_SUCCESS;
}

static int ai_vdec_create(int channel)
{
    VDEC_CHN VdecChn;
    HI_S32 s32Ret = HI_FAILURE;
    
    if (channel < 0 || channel >= 64) {
        return HI_FAILURE;
    }

    VdecChn = channel;
    if (channel >= 32 && channel < 64) 
        transfer_buff[VdecChn] = malloc(IMAGE_MAX_W*IMAGE_MAX_H*3/2);
    s32Ret = AI_VDEC_CreateChannel(VdecChn, PT_JPEG, 4096, 4096);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VDEC_Create: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static int ai_vdec_destroy(int channel)
{
    VDEC_CHN VdecChn;
    HI_S32 s32Ret = HI_FAILURE;

    if (channel < 0 || channel >= 64) {
        return HI_FAILURE;
    }

    VdecChn = channel;

    AI_VDEC_DestroyChannel(VdecChn);

    return HI_SUCCESS;
}

static int ai_vpss_init(void)
{
    memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
    memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));

    stVpssGrpAttr.u32MaxW                       = VIDEO_MAX_W;
    stVpssGrpAttr.u32MaxH                       = VIDEO_MAX_H;
    stVpssGrpAttr.enPixelFormat                 = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.enDynamicRange                = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate   = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate   = -1;
    stVpssGrpAttr.bNrEn                         = HI_FALSE;

    stVpssChnAttr.enChnMode                     = VPSS_CHN_MODE_USER;
    stVpssChnAttr.u32Width                      = VIDEO_MAX_W;
    stVpssChnAttr.u32Height                     = VIDEO_MAX_H;
    stVpssChnAttr.enVideoFormat                 = VIDEO_FORMAT_LINEAR;
    stVpssChnAttr.enPixelFormat                 = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssChnAttr.enDynamicRange                = DYNAMIC_RANGE_SDR8;
    stVpssChnAttr.enCompressMode                = COMPRESS_MODE_NONE;
    stVpssChnAttr.stFrameRate.s32SrcFrameRate   = -1;
    stVpssChnAttr.stFrameRate.s32DstFrameRate   = -1;
    stVpssChnAttr.bMirror                       = HI_FALSE;
    stVpssChnAttr.bFlip                         = HI_FALSE;
    stVpssChnAttr.u32Depth                      = 2;
    stVpssChnAttr.stAspectRatio.enMode          = ASPECT_RATIO_NONE;

    return HI_SUCCESS;
}

static int ai_vpss_create(int channel)
{
    VB_POOL_CONFIG_S stVbPoolCfg;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VB_POOL hVbPool;
    HI_S32 s32Ret = HI_FAILURE;

    if (channel < 0 || channel > 64) {
        return HI_FAILURE;
    }

    VpssGrp = channel;
    VpssChn = 0;
    if (channel < 32)
    {
        stVpssChnAttr.u32Height = VIDEO_MAX_H;
        stVpssChnAttr.u32Width = VIDEO_MAX_W;
        stVpssGrpAttr.u32MaxH = VIDEO_MAX_H;
        stVpssGrpAttr.u32MaxW = VIDEO_MAX_W;
    }
    else
    {
        stVpssChnAttr.u32Height = IMAGE_MAX_H;
        stVpssChnAttr.u32Width = IMAGE_MAX_W;
        stVpssGrpAttr.u32MaxH = IMAGE_MAX_H;
        stVpssGrpAttr.u32MaxW = IMAGE_MAX_W;
    }
    s32Ret = AI_COMM_VPSS_CreateChannel(VpssGrp, VpssChn, &stVpssGrpAttr, &stVpssChnAttr, HI_TRUE);
    if (HI_SUCCESS != s32Ret) {
        printf("AI_COMM_VPSS_Create: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));

    stVbPoolCfg.u64BlkSize  = COMMON_GetPicBufferSize(astAiVdec[VpssGrp].u32Width, astAiVdec[VpssGrp].u32Height,
                                                      PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8,
                                                      COMPRESS_MODE_NONE, 0);
    stVbPoolCfg.u32BlkCnt   = 32;
    stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;

    hVbPool = HI_MPI_VB_CreatePool(&stVbPoolCfg);
    if (VB_INVALID_POOLID == hVbPool) {
        printf("HI_MPI_VB_CreatePool: %#x\n", hVbPool);

        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_AttachVbPool(VpssGrp, VpssChn, hVbPool);
    if (HI_SUCCESS != s32Ret) {
        printf("HI_MPI_VPSS_AttachVbPool: %#x\n", s32Ret);

        return HI_FAILURE;
    }

    astAiVdec[VpssGrp].hVbPool = hVbPool;

    return HI_SUCCESS;
}

static int ai_vpss_destroy(int channel)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VB_POOL hVbPool;
    HI_S32 s32Ret = HI_FAILURE;

    if (channel < 0 || channel > 64) {
        return HI_FAILURE;
    }

    VpssGrp = channel;
    VpssChn = 0;

    hVbPool = astAiVdec[VpssGrp].hVbPool;

    HI_MPI_VPSS_DetachVbPool(VpssGrp, VpssChn);

    HI_MPI_VB_DestroyPool(hVbPool);

    AI_COMM_VPSS_DestroyChannel(VpssGrp, VpssChn);

    return HI_SUCCESS;
}

int ai_init(void)
{
    int ret = 0;

    AI_SYS_Exit();

    ret = AI_SYS_Init();
    if (0 != ret) {
        return ret;
    }

    ret = AI_VDEC_Init();
    if (0 != ret) {
        return ret;
    }

    ret = AI_VPSS_Init();
    if (0 != ret) {
        return ret;
    }

    for (int i = 0; i < 16; i++) {
        astChnInfo[i].enType = AI_CHN_TYPE_H264D_VPSS;
        astChnInfo[i].s32Chn = i;
        astChnInfo[i].u32Width = 1920;//1280;
        astChnInfo[i].u32Height = 1080;//720;
        astChnInfo[i].u32VpssWidth = 1920;
        astChnInfo[i].u32VpssHeight = 1080;
    }

    for (int i = 16; i < 32; i++) {
        astChnInfo[i].enType = AI_CHN_TYPE_H264D_VPSS;
        astChnInfo[i].s32Chn = i;
        astChnInfo[i].u32VpssWidth = 1920;
        astChnInfo[i].u32VpssHeight = 1080;
    }

    for (int i = 33; i < 64; i++) {
        astChnInfo[i].enType = AI_CHN_TYPE_JPEGD;
        astChnInfo[i].s32Chn = i;
    }

    return ret;
}

void ai_exit(void)
{
    AI_SYS_Exit();
}

int ai_video_decode_init(int channel)
{
    if (ai_sdk_create(&astChnInfo[channel])) {
        return -1;
    }

    return 0;
}

int ai_video_decode_exit(int channel)
{
    printf("280\n");
    if (ai_sdk_destroy(&astChnInfo[channel])) {
        return -1;
    }

    return 0;
}




int ai_video_decode_send_frame(int channel, char *frame_data, int frame_length, int isSpsFrame)
{
    //printf("ai_video_decode_send_frame start\n");
    if (0 && isSpsFrame) {
        HI_U32 u32Width = 0;
        HI_U32 u32Height = 0;
        AI_Util_GetH264Info(frame_data, frame_length, &u32Width, &u32Height);
        printf("4int:%d,%d,%d,%d \n", u32Width, astChnInfo[channel].u32Width, u32Height, astChnInfo[channel].u32Height);
        if (u32Width != astChnInfo[channel].u32Width || u32Height != astChnInfo[channel].u32Height) {
            printf("299\n");
            astChnInfo[channel].u32Width = u32Width;
            astChnInfo[channel].u32Height = u32Height;
            ai_sdk_destroy(&astChnInfo[channel]);
            ai_sdk_create(&astChnInfo[channel]);
        }
    }
    astChnInfo[channel].pStream = (unsigned char *) frame_data;
    astChnInfo[channel].u32StreamLen = frame_length;

    if (ai_sdk_send(&astChnInfo[channel])) {
        return -1;
    }
    //printf("ai_video_decode_send_frame return\n");
    return 0;
}

int ai_video_decode_receive_frame(int channel, AI_IMAGE_S **frame_image)
{
    //printf("ai_video_decode_receive_frame start\n");
    if (ai_sdk_get(&astChnInfo[channel])) {
        return -1;
    }

    *frame_image = malloc(sizeof(AI_IMAGE_S));
    if (NULL == *frame_image) {
        perror("malloc()");

        ai_sdk_release(&astChnInfo[channel]);

        return HI_FAILURE;
    }

    memset(*frame_image, 0, sizeof(AI_IMAGE_S));

    //(*frame_image)->width = astChnInfo[channel].pstVFrame->stVFrame.u32Stride[0];
    (*frame_image)->width = astChnInfo[channel].pstVFrame->stVFrame.u32Width;
    (*frame_image)->height = astChnInfo[channel].pstVFrame->stVFrame.u32Height;

    (*frame_image)->stride[0] = astChnInfo[channel].pstVFrame->stVFrame.u32Stride[0];
    (*frame_image)->stride[1] = astChnInfo[channel].pstVFrame->stVFrame.u32Stride[1];
    (*frame_image)->stride[2] = astChnInfo[channel].pstVFrame->stVFrame.u32Stride[2];

    (*frame_image)->phy_addr[0] = (void *) astChnInfo[channel].pstVFrame->stVFrame.u64PhyAddr[0];
    (*frame_image)->phy_addr[1] = (void *) astChnInfo[channel].pstVFrame->stVFrame.u64PhyAddr[1];
    (*frame_image)->phy_addr[2] = (void *) astChnInfo[channel].pstVFrame->stVFrame.u64PhyAddr[2];

    HI_U64 u64PhyAddr = 0;
    HI_U32 u32Size = 0;
    HI_VOID *pVirAddr = NULL;

    u64PhyAddr = (HI_U64) (*frame_image)->phy_addr[0];
    u32Size = ((*frame_image)->phy_addr[1] - (*frame_image)->phy_addr[0]) * 3 / 2;
    pVirAddr = HI_MPI_SYS_Mmap(u64PhyAddr, u32Size);

    (*frame_image)->vir_addr[0] = pVirAddr;
    (*frame_image)->vir_addr[1] = pVirAddr + ((*frame_image)->phy_addr[1] - (*frame_image)->phy_addr[0]);
    (*frame_image)->vir_addr[2] = pVirAddr + ((*frame_image)->phy_addr[1] - (*frame_image)->phy_addr[0]);

    (*frame_image)->channel = channel + 1;
    (*frame_image)->frame_info = astChnInfo[channel].pstVFrame;
    //printf("ai_video_decode_receive_frame end\n");
    return 0;
}

int ai_video_decode_release_frame(AI_IMAGE_S *frame_image)
{
    int channel = 0;
    channel = frame_image->channel;

    HI_U64 u64PhyAddr = 0;
    HI_U32 u32Size = 0;
    HI_VOID *pVirAddr = NULL;

    pVirAddr = frame_image->vir_addr[0];
    u32Size = (frame_image->vir_addr[1] - frame_image->vir_addr[0]) * 3 / 2;
    HI_MPI_SYS_Munmap(pVirAddr, u32Size);

    AI_CHN_INFO_S stChnInfo = {0};

    stChnInfo.enType = astChnInfo[channel].enType;
    stChnInfo.s32Chn = astChnInfo[channel].s32Chn;
    stChnInfo.pstVFrame = frame_image->frame_info;

    ai_sdk_release(&stChnInfo);
    free(frame_image);

    return 0;
}

int ai_frame_attach_bitmap(int channel, char *bitmap_path, RGN_OVERLAY_PARA * param)
{
    BITMAP_S stBitmap;
    printf("prepare to load bit map\r\n");
    HI_S32 s32Ret = AI_Load_Bmp(bitmap_path, &stBitmap, HI_FALSE, 0);
	if (s32Ret != HI_SUCCESS)
    {
        printf(" bitmap load fail. value=0x%x.", s32Ret);
        return s32Ret;
    }
    s32Ret = AI_RGN_ATTACH_BITMAP_TO_CHN(channel, &stBitmap,param);
    return s32Ret;
}

int ai_frame_detach(int channel)
{
    HI_S32 s32Ret = AI_RGN_Destory_Region(channel);
    return s32Ret;
}

void ai_frame_vdec_vpss_connect(int channel, HI_BOOL on_or_off)
{
    AI_COMM_VDEC_ENBALEVPSS(channel,on_or_off);
}

int ai_frame_vpss_setcrop(int channel,RECT_S crop_para)
{
    RECT_S new_para = crop_para;
    if (new_para.s32X % ALIGN_W_MIN != 0)
        new_para.s32X = new_para.s32X / ALIGN_W_MIN * ALIGN_W_MIN + ALIGN_W_MIN;
    else new_para.s32X = new_para.s32X;
    if (new_para.s32Y % ALIGN_H_MIN != 0)
        new_para.s32Y = new_para.s32Y + 1;
    else new_para.s32Y = new_para.s32Y;
    if (new_para.u32Height % ALIGN_H_MIN != 0)
        new_para.u32Height = new_para.u32Height + 1;
    else new_para.u32Height = new_para.u32Height;
    if (new_para.u32Width % ALIGN_W_MIN !=0)
        new_para.u32Width = new_para.u32Width / ALIGN_W_MIN * ALIGN_W_MIN + ALIGN_W_MIN;
    else new_para.u32Width = new_para.u32Width;
    printf("vgs set crop xywh: %d,%d,%d,%d\r\n",new_para.s32X,new_para.s32Y,new_para.u32Width,new_para.u32Height);
    HI_S32 ret = AI_COMM_VPSS_SetChnCrop(channel,0,new_para);
    return ret;
}

int ai_frame_vpss_setframesize(int channel,SIZE_S stSize)
{
    HI_S32 ret = 0;
    if (stSize.u32Width % ALIGN_W_MIN != 0)
        stSize.u32Width =  stSize.u32Width / ALIGN_W_MIN * ALIGN_W_MIN + ALIGN_W_MIN;
    if (stSize.u32Height % ALIGN_H_MIN != 0)
        stSize.u32Height =  stSize.u32Height + 1;
    //printf("vgs set size %d,%d\r\n",stSize.u32Width,stSize.u32Height);
    if (stSize.u32Width <= 0 || stSize.u32Height <= 0)
        AI_COMM_VDEC_AUTOSIZE(channel,HI_TRUE);
    else 
    {
        AI_COMM_VDEC_AUTOSIZE(channel,HI_FALSE);
        ret = AI_COMM_VPSS_SetChnSize(channel, 0,stSize);
    }
    return ret;
}

int ai_frame_ive_perstrans(AI_IMAGE_S* img,AI_IMAGE_S ** img_res, PERSTRANS_PARA attr)
{
    AI_COMM_IVE_PerspTrans(img,img_res ,attr);
}

int ai_frame_ive_affine(AI_IMAGE_S* img,AI_IMAGE_S ** img_res, PERSTRANS_PARA attr)
{
    AI_COMM_IVE_Affine(img,img_res ,attr);
}

int ai_video_start_vo(int channel)
{
    AI_VO_SET_ATTR attr;
    AI_VO_SET_ATTR new_attr;
    attr.ChnNum = 1;
    attr.Disp_Rect.s32X = attr.Disp_Rect.s32Y = 0;
    attr.Disp_Rect.u32Width = 1920;
    attr.Disp_Rect.u32Height = 1080;
    attr.Vodev = 0; //SAMPLE_VO_DEV_UHD;
    attr.VoIntfType = VO_INTF_HDMI;
    AI_START_VO(&attr);
    /*AI_START_VO()
    new_attr.ChnNum = 1;
    new_attr.Disp_Rect.s32X = new_attr.Disp_Rect.s32Y = 0;
    new_attr.Disp_Rect.u32Width = 1920;
    new_attr.Disp_Rect.u32Height = 1080;
    new_attr.Vodev = SAMPLE_VO_DEV_UHD;
    new_attr.VoIntfType = VO_INTF_HDMI;
    AI_START_VO(&new_attr);*/
    AI_COMM_VPSS_Bind_VO(channel,0,0,0);
}

int ai_video_stop_vo(int channel)
{
    AI_VO_SET_ATTR attr;
    attr.ChnNum = 1;
    attr.Disp_Rect.s32X = attr.Disp_Rect.s32Y = 0;
    attr.Disp_Rect.u32Width = 1920;
    attr.Disp_Rect.u32Height = 1080;
    attr.Vodev = 0; //SAMPLE_VO_DEV_UHD;
    attr.VoIntfType = VO_INTF_HDMI;
    AI_COMM_VPSS_UnBind_VO(channel,0,0,0);
    AI_STOP_VO(&attr);
    
}
