#ifndef AI_vpu_SDK_H
#define AI_vpu_SDK_H

#include <stdio.h>

#define MAX_BOX_NUM 50;

typedef enum
{
    AI_DEVICE_TYPE_PC, // 英特尔平台(Intel)
    AI_DEVICE_TYPE_NV, // 英伟达平台(Nvidia)
    AI_DEVICE_TYPE_HI, // 海思平台(HiSilicon)
    AI_DEVICE_TYPE_RK, // 瑞芯微平台(Rockchip)
    AI_DEVICE_TYPE_HB, // 地平线平台(Horizon)
} AI_DEVICE_TYPE_E;

typedef struct
{
    int image_width;
    int image_height;
    int aligned_width;
    int aligned_height;
    int data_type;
    void *data_frame;
} AI_IMAGE_FRAME_S;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
    int resize_h;
    int resize_w;
} AI_VPS_INFO_S;

typedef struct
{
    AI_IMAGE_FRAME_S *frame;
    int length;
    AI_VPS_INFO_S *info_arr;

} AI_VPS_INPUT;

/**
 * 判断该函数是否在平台下可用,如果可用,是硬件方式实现,还是软件方式实现
 * @param func_name 函数名 [输入参数]
 * @param flag 函数可用则为1,否则为0 [输出参数]
 * @param ishardware 硬件方式实现则为1,软件方式实现为0 [输出参数]
 * @return
 */
// int ai_system_is_in_platform(char *func_name, int *flag, int *ishardware);

/**
 * 整个系统初始化
 * @return
 */
int ai_system_init(void);

/**
 * 整个系统退出
 */
void ai_system_exit(void);

/**
 * 获取用的底层是哪个平台
 * @param device_type [输出参数]
 * @return
 */
int ai_system_get_device_type(AI_DEVICE_TYPE_E *device_type);

/**
 * 获得能够同时启动视频的最大通道数
 * @param max_channel [输出参数]
 * @return
 */
int ai_video_get_max_channel(int *max_channel);

/**
 * 视频解码通道初始化
 * @param channel [输入参数]
 * @return
 */
int ai_video_decode_create(int channel);

/**
 * 视频解码通道销毁
 * @param channel [输入参数]
 * @return
 */
int ai_video_decode_destroy(int channel);

/**
 * 发送到视频解码通道
 * @param channel [输入参数]
 * @param stream [输入参数]
 * @param length [输入参数]
 * @return
 */
int ai_video_decode_send(int channel, void *stream, int length);

/**
 * 从视频解码通道里获取
 * @param channel [输入参数]
 * @param frame [输出参数]
 * @return
 */
int ai_video_decode_get(int channel, AI_IMAGE_FRAME_S **frame);

/**
 * 图像解码
 * @param stream [输入参数]
 * @param length [输入参数]
 * @param frame [输出参数]
 * @return
 */
int ai_image_decode(void *stream, int length, AI_IMAGE_FRAME_S **frame);

/**
 * 图像编码
 * @param frame [输入参数]
 * @param save_path [输入参数]
 * @return
 */
int ai_image_encode(AI_IMAGE_FRAME_S *frame, char * save_path);


/**
 * vps 实现crop和resize功能,支持一个frame输入,多个frame输出
 * @param input 包含需要处理的frame, 输出的frame的数量, 每个输出frame对应的处理方式 [输入参数]
 * @param frame 输出的frame的首地址 [输出参数]
 * @param lenght 输出的frame的数量 [输出参数]
 * @return
 */
int ai_image_vps(AI_VPS_INPUT *input, AI_IMAGE_FRAME_S **frame, int *length);

/**
 * 结果释放
 * @param frame [输入参数]
 * @return
 */
int ai_frame_release(AI_IMAGE_FRAME_S **frame);






/**
 * 模型初始化
 * @param data_dir 存放日志目录log，模型目录model，配置文件目录conf，测试图片目录image，测试视频目录video
 * @param model_name 模型名称
 * @return 0成功，100:模型不存在， 101:权值文件不存在， 102：npu 储存不足
 */
int ai_model_init(char *data_dir， char *model_name);

/**
 * 分类
 * @param model_name 模型名称
 * @param frame 待分类图片
 * @scores 得分列表，需要调用者提前分配内存
 * @return  0:成功 102：npu 储存不足， 103：未初始化
 */
int ai_classify(char *model_name, AI_IMAGE_FRAME_S *frame, float *scores);


/**
 * 检测结果方框列表
 * @param x,y,w,h 方框坐标
 * @param scores 此方框每个类别得分
 */
typedef struct BOX
{
    int x,y,w,h;
    float *scores;
} ST_BOX;

/**
 * 方框检测
 * @param model_name 模型名称
 * @param frame 待分类图片
 * @param boxes 检测结果方框列表，需要调用者提前分配存储空间
 * @return 0：成功， 102：npu 储存不足， 103：未初始化
 */
int ai_detect(char *model_name, AI_IMAGE_FRAME_S *frame, ST_BOX boxes[MAX_BOX_NUM], int *pnbox);




#endif //AI_vpu_SDK_H
