#include "sample_comm_ive.h"

#define SAMPLE_IVE_GMM2_MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct hiSAMPLE_IVE_GMM2_S
{
    IVE_SRC_IMAGE_S astSrc[2];
    IVE_DST_IMAGE_S stFg;
    IVE_DST_IMAGE_S stBg;
    IVE_SRC_IMAGE_S    stFactor;
    IVE_DST_IMAGE_S    stMatchModelInfo;
    IVE_SRC_IMAGE_S stFgMask;
    IVE_SRC_IMAGE_S stLastImg;
    IVE_DST_IMAGE_S stDiffImg;
    IVE_DST_IMAGE_S stMagImg;
    IVE_DST_IMAGE_S stCurNormMag;
    IVE_DST_IMAGE_S stLastNormMag;
    IVE_DST_MEM_INFO_S stModel;
    IVE_DST_MEM_INFO_S stBlob;
    IVE_GMM2_CTRL_S stGmm2Ctrl;

    IVE_IMAGE_S    stImg1;
    IVE_IMAGE_S    stImg2;
    IVE_CCL_CTRL_S  stCclCtrl;
    IVE_DILATE_CTRL_S stDilateCtrl;
    IVE_ERODE_CTRL_S stErodeCtrl;



    FILE *pFpSrc;
    FILE *pFpFg;
    FILE *pFpBg;

    HI_U32 u32FrmNum;
    HI_S32 s32CurIdx;
    SAMPLE_RECT_ARRAY_S stRegion;
} SAMPLE_IVE_GMM2_S;

HI_S32 SAMPLE_IVE_Gmm2_Init(SAMPLE_IVE_GMM2_S *pstGmm2,HI_U32 u32Width,HI_U32 u32Height, HI_CHAR *pchSrcFileName,
            HI_CHAR *pchFgFileName,HI_CHAR *pchBgFileName);
HI_S32 SAMPLE_IVE_Gmm2Proc(SAMPLE_IVE_GMM2_S *pstGmm2);
//HI_VOID SAMPLE_IVE_Gmm2(SAMPLE_IVE_GMM2_S *pstGmm2);
//HI_S32 SAMPLE_IVE_Image_Gmm2Proc(SAMPLE_IVE_GMM2_S *pstGmm2, VIDEO_FRAME_INFO_S* pstExtFrmInfo);
HI_S32 SAMPLE_IVE_Image_Gmm2Proc(SAMPLE_IVE_GMM2_S *pstGmm2, VIDEO_FRAME_INFO_S* pstExtFrmInfo);

HI_VOID SAMPLE_IVE_Gmm2_Uninit(SAMPLE_IVE_GMM2_S *pstGmm2);