#ifndef AI_NPU_SDK_H
#define AI_NPU_SDK_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "ai_global.h"
#include "aibox.h"

/**
 * 模型初始化
 * @param data_dir 存放日志目录log，模型目录model，配置文件目录conf，测试图片目录image，测试视频目录video
 * @param model_name 模型名称
 * @return 0成功，100:模型不存在， 101:权值文件不存在， 102：npu 储存不足
 */
//int ai_model_init(char *data_dir, char *model_name);
int ai_model_init(AI_DETECT_ARGS_ST argst);




/**
 * 方框检测
 * @param model_name 模型名称
 * @param frame 待分类图片
 * @param boxes 检测结果方框列表，需要调用者提前分配存储空间
 * @return 0：成功， 102：npu 储存不足， 103：未初始化
 */
int ai_detect(char *model_name, AI_IMAGE_S *frame, ST_BOX boxes[MAX_BOX_NUM], int *pnbox);
//int ai_detect(char *model_name, AI_IMAGE_FRAME_S *pframe, ST_BOX boxes[MAX_BOX_NUM], int*pnbox);



#ifdef __cplusplus
}
#endif

#endif //AI_vpu_SDK_H
