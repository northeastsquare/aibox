/**
 * 模型初始化
 * @param data_dir 存放日志目录log，模型目录model，配置文件目录conf，测试图片目录image，测试视频目录video
 * @param model_name 模型名称
 * @return 0成功，100:模型不存在， 101:权值文件不存在， 102：npu 储存不足
 */
int model_init(char *data_dir， char *model_name);

/**
 * 分类
 * @param model_name 模型名称
 * @param frame 待分类图片
 * @scores 得分列表，需要调用者提前分配内存
 * @return  0:成功 102：npu 储存不足， 103：未初始化
 */
int classify(char *model_name, AI_IMAGE_FRAME_S *frame, float *scores);


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
int detect(char *model_name, AI_IMAGE_FRAME_S *frame, ST_BOX *boxes);


