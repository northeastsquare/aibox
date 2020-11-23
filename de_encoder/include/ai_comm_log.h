#ifndef AIBOX_SDK_AI_COMM_LOG_H
#define AIBOX_SDK_AI_COMM_LOG_H

#include "ai_common.h"

enum AI_FUNC_LOG_INDEX
{
    AI_FUNC_LOG_INDEX = 0,

    AI_INIT_LOG_INDEX,
    AI_EXIT_LOG_INDEX,

    AI_VIDEO_DECODE_INIT_LOG_INDEX,
    AI_VIDEO_DECODE_EXIT_LOG_INDEX,
    AI_VIDEO_DECODE_SEND_FRAME_LOG_INDEX,
    AI_VIDEO_DECODE_RECEIVE_FRAME_LOG_INDEX,
    AI_VIDEO_DECODE_RELEASE_FRAME_LOG_INDEX,

    AI_COMM_SYS_LOG_INDEX,
    AI_COMM_VENC_LOG_INDEX,
    AI_COMM_VDEC_LOG_INDEX,
    AI_COMM_VPSS_LOG_INDEX,

    AI_SYS_LOG_INDEX,
    AI_VENC_LOG_INDEX,
    AI_VDEC_LOG_INDEX,
    AI_VPSS_LOG_INDEX,
};

enum AI_FUNC_LOG_FLAG
{
    AI_DISABLE_FUNC_LOG_FLAG                = (unsigned long) 0 << AI_FUNC_LOG_INDEX,
    AI_ENABLE_FUNC_LOG_FLAG                 = (unsigned long) 1 << AI_FUNC_LOG_INDEX,

    AI_INIT_LOG_FLAG                        = (unsigned long) 1 << AI_INIT_LOG_INDEX,
    AI_EXIT_LOG_FLAG                        = (unsigned long) 1 << AI_EXIT_LOG_INDEX,

    AI_VIDEO_DECODE_INIT_LOG_FLAG           = (unsigned long) 1 << AI_VIDEO_DECODE_INIT_LOG_INDEX,
    AI_VIDEO_DECODE_EXIT_LOG_FLAG           = (unsigned long) 1 << AI_VIDEO_DECODE_EXIT_LOG_INDEX,
    AI_VIDEO_DECODE_SEND_FRAME_LOG_FLAG     = (unsigned long) 1 << AI_VIDEO_DECODE_SEND_FRAME_LOG_INDEX,
    AI_VIDEO_DECODE_RECEIVE_FRAME_LOG_FLAG  = (unsigned long) 1 << AI_VIDEO_DECODE_RECEIVE_FRAME_LOG_INDEX,
    AI_VIDEO_DECODE_RELEASE_FRAME_LOG_FLAG  = (unsigned long) 1 << AI_VIDEO_DECODE_RELEASE_FRAME_LOG_INDEX,

    AI_COMM_SYS_LOG_FLAG                    = (unsigned long) 1 << AI_COMM_SYS_LOG_INDEX,
    AI_COMM_VENC_LOG_FLAG                   = (unsigned long) 1 << AI_COMM_VENC_LOG_INDEX,
    AI_COMM_VDEC_LOG_FLAG                   = (unsigned long) 1 << AI_COMM_VDEC_LOG_INDEX,
    AI_COMM_VPSS_LOG_FLAG                   = (unsigned long) 1 << AI_COMM_VPSS_LOG_INDEX,

    AI_SYS_LOG_FLAG                         = (unsigned long) 1 << AI_SYS_LOG_INDEX,
    AI_VENC_LOG_FLAG                        = (unsigned long) 1 << AI_VENC_LOG_INDEX,
    AI_VDEC_LOG_FLAG                        = (unsigned long) 1 << AI_VDEC_LOG_INDEX,
    AI_VPSS_LOG_FLAG                        = (unsigned long) 1 << AI_VPSS_LOG_INDEX,
};

extern int ai_func_log_flag[];

#define ai_printf(fmt...)                                       \
    do {                                                        \
        if (ai_func_log_flag[ai_func_log_index] == 1) {         \
            printf("[%s - %d]: ", __FUNCTION__, __LINE__);      \
                                                                \
            printf(fmt);                                        \
        }                                                       \
    } while(0)

int ai_set_func_log_flag(unsigned long flag);

#endif //AIBOX_SDK_AI_COMM_LOG_H
