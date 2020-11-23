#ifndef AIBOX_SDK_AISDK_H
#define AIBOX_SDK_AISDK_H

#include "ai_common.h"
#include "ai_comm_sys.h"
#include "ai_comm_venc.h"
#include "ai_comm_vdec.h"
#include "ai_comm_vpss.h"
#include "ai_image_mmz.h"
#include "ai_comm_ive.h"
#include "ai_save_image.h"
#include "ai_comm_rgn.h"
#include "ai_sys.h"
#include "ai_venc.h"
#include "ai_vdec.h"
#include "ai_vpss.h"
#include "ai_comm_vo.h"

const int VIDEO_MAX_W = 3840;
const int VIDEO_MAX_H = 2160;
const int IMAGE_MAX_W = 1920*2;
const int IMAGE_MAX_H = 1080*2;





int ai_init(void);

void ai_exit(void);

int ai_video_decode_init(int channel);

int ai_video_decode_exit(int channel);

int ai_video_decode_send_frame(int channel, char *frame_data, int frame_length, int isSpsFrame);

int ai_video_decode_receive_frame(int channel, AI_IMAGE_S **frame_image);

int ai_video_decode_release_frame(AI_IMAGE_S *frame_image);

int ai_frame_attach_bitmap(int channel, char *bitmap_path, RGN_OVERLAY_PARA * param);

int ai_frame_detach(int channel);

void ai_frame_vdec_vpss_connect(int channel, HI_BOOL on_or_off);

int ai_frame_vpss_setcrop(int channel,RECT_S crop_para);

int ai_frame_vpss_setframesize(int channel,SIZE_S stSize);

int ai_frame_ive_perstrans(AI_IMAGE_S* img,AI_IMAGE_S ** img_res,PERSTRANS_PARA attr);

int ai_frame_ive_affine(AI_IMAGE_S* img,AI_IMAGE_S ** img_res, PERSTRANS_PARA attr);

int ai_video_start_vo(int channel);

int ai_video_stop_vo(int channel);

int ai_video_vpss_init(int channel);
#endif //AIBOX_SDK_AISDK_H
