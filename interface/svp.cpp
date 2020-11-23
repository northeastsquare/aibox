#include <sstream>
#include <fstream>
using namespace std;

static std::unique_ptr<CSvp> g_svp;

/**
 * 模型初始化
 * @param data_dir 存放日志目录log，模型目录model，配置文件目录conf，测试图片目录image，测试视频目录video
 * @param model_name 模型名称
 * @return 0成功，100:模型不存在， 101:权值文件不存在， 102：npu 储存不足
 */
int ai_init(char *data_dir, char *model_name, float fConfThresh, bool bNMS, float fNmsThresh){
    int nret;
    string sDataDir(data_dir);
    string sModelName(model_name);
    nret = g_svp->DetectorInit(sDataDir, sModelName, fConfThresh, bNMS, fNmsThresh);
    return nret;
}

/**
 * 方框检测
 * @param model_name 模型名称
 * @param frame 待分类图片
 * @param inout boxes 检测结果方框列表，需要调用者提前分配存储空间
 * @param 
 * @return 0：成功， 102：npu 储存不足， 103：未初始化
 */
int ai_detect(char *model_name, AI_IMAGE_FRAME_S *pframe, ST_BOX boxes[MAX_BOX_NUM], int*pnbox){    
    string sModelName(model_name);
    VIDEO_FRAME_INFO_S *pstVFrame = (VIDEO_FRAME_INFO_S *)pframe->data_frame;
    vector<ST_BOX> vBox;
    int nret = g_svp->Detect(sModelName, pstVFrame, vBox);
    *pnbox = min(MAX_BOX_NUM, vBox.size());
    for(int i = 0; i< *pnbox, i++){
        boxes[i] = vBox[i];
    }
    return nret;
}
