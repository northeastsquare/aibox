#ifndef _AI_GLOBAL_H_
#define _AI_GLOBAL_H_

#define MAX_MODEL_NAME_LEN 50
#define MAX_BOX_NUM 50
#define DETECT_CLASS_NUM 80
#define MAX_PATH_LEN 200

typedef struct detect_args_struct{
    int image_width;
    int image_height;
    int class_num;
    int do_nms;
    float nms_thresh;
    float conf_thresh;//类别得分阈值
    char data_dir[MAX_PATH_LEN];
    char model_name[MAX_MODEL_NAME_LEN];
    
} AI_DETECT_ARGS_ST;

/**
 * 检测结果方框列表
 * @param x,y,w,h 方框坐标
 * @param scores 此方框每个类别得分
 */
typedef struct BOX
{
    int clss_id;
    float conf;
    int x,y,w,h;
} ST_BOX;


#endif