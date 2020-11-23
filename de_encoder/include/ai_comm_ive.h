
#ifndef AIBOX_SDK_AI_COMM_IVE_H
#define AIBOX_SDK_AI_COMM_IVE_H

#include "ai_common.h"

typedef struct hiPOINT_F
{
    HI_FLOAT s32X;
    HI_FLOAT s32Y;
}AI_POINT_F;

typedef struct pers_trans_para
{
    HI_U32 points_number;
    AI_POINT_F * src_points;
    AI_POINT_F * dst_points;
}PERSTRANS_PARA;

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus

int do_warp_by_opencv(AI_IMAGE_S * input, AI_IMAGE_S ** img_res,PERSTRANS_PARA trans_attr,int type);

HI_S32 AI_COMM_IVE_PerspTrans(AI_IMAGE_S * input,AI_IMAGE_S ** img_res,PERSTRANS_PARA trans_attr);

HI_S32 AI_COMM_IVE_Affine(AI_IMAGE_S * img, AI_IMAGE_S ** img_res,PERSTRANS_PARA trans_attr);


#ifdef __cplusplus
}
#endif  //@__cplusplus






#endif //AIBOX_SDK_AI_COMM_IVE_H