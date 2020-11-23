
extern "C"{
#include "sample_ive_gmm2.h"
}
int main(){
    SAMPLE_IVE_GMM2_S s_stGmm2;
    HI_S32                 s32Ret;
    HI_U32                 u32Width  = 352;
    HI_U32                 u32Height = 288;
    HI_CHAR                *pchSrcFileName = "../../data/videos/gmm2_352x288_sp400_frm1000.yuv";
    HI_CHAR                *pchFgFileName = "../../data/videos/fg_352x288_sp400.yuv";
    HI_CHAR                *pchBgFileName = "../../data/videos/bg_352x288_sp400.yuv";

    memset(&s_stGmm2,0,sizeof(s_stGmm2));
    SAMPLE_COMM_IVE_CheckIveMpiInit();

    s32Ret = SAMPLE_IVE_Gmm2_Init(&s_stGmm2, u32Width,u32Height,pchSrcFileName,
                        pchFgFileName,pchBgFileName);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM2_FAIL,
        "Error(%#x),SAMPLE_IVE_Gmm2_Init failed!\n", s32Ret);

    s32Ret = SAMPLE_IVE_Gmm2Proc(&s_stGmm2);
    if (HI_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("Process success!\n");
    }

    SAMPLE_IVE_Gmm2_Uninit(&s_stGmm2);
    memset(&s_stGmm2,0,sizeof(s_stGmm2));
    return 0;
GMM2_FAIL:
    SAMPLE_COMM_IVE_IveMpiExit();

}