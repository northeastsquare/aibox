#ifndef AI_NPU_SDK_H
#define AI_NPU_SDK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ai_vpu_sdk.h"

typedef int AI_MODEL_HANDLE;

typedef enum AI_MODEL_TYPE_ENUM
{
    AI_MODEL_TYPE_SSD, 
    AI_MODEL_TYPE_YOLOV3,

} AI_MODEL_TYPE_E;

typedef struct AI_BOX_ST
{
    int class_id;
    float confidence;
    float x, y, w, h;
} AI_BOX_S;

/**
 * 模型初始化
 * @param model_type 模型类型 [输入参数]
 * @param model_file 模型文件 [输入参数]
 * @param image_width 图片宽度 [输入参数]
 * @param image_height 图片高度 [输入参数]
 * @param class_number 类别数量 [输入参数]
 * @param model_handle 模型句柄 [输出参数]
 * @return
 */
//int ai_model_init(AI_MODEL_TYPE_E model_type, char* model_file, int image_width, int image_height, int class_number, AI_MODEL_HANDLE* model_handle);
int ai_model_init(AI_MODEL_TYPE_E model_type, char* model_file, int net_width, int net_height, int class_number \
        , int npu_core_id, AI_MODEL_HANDLE* model_handle);

/**
 * 模型释放
 * @param model_handle 模型句柄 [输入参数]
 * @return
 */
int ai_model_deinit(AI_MODEL_HANDLE model_handle);

/**
 * 模型释放
 * @param model_handle 模型句柄 [输入参数]
 * @param frame 模型句柄 [输入参数]
 * @param confidence_threshhold 模型句柄 [输入参数]
 * @param nms_threshhold 模型句柄 [输入参数]
 * @param boxes 框数组 [输入参数]
 * @param box_lenght 框长度 [输入参数]
 * @return
 */
int ai_model_yolov3(AI_MODEL_HANDLE model_handle, AI_IMAGE_FRAME_S* frame, float confidence_threshhold, float nms_threshhold, AI_BOX_S** boxes, int* box_length);

/**
 * 内存释放
 * @param p [输入参数]
 * @return
 */
int ai_model_delete(void* p);


#ifdef __cplusplus
}
#endif

#endif 
