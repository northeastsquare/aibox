#include <sstream>
#include <fstream>
#include <memory>
#include "ai_npu_sdk.h"
#include "CSvp.h"
using namespace std;

static std::unique_ptr<CSvp> g_svp;

/**
 * 模型初始化
 * @param data_dir 存放日志目录log，模型目录model，配置文件目录conf，测试图片目录image，测试视频目录video
 * @param model_name 模型名称
 * @return 0成功，100:模型不存在， 101:权值文件不存在， 102：npu 储存不足
 */
int ai_model_init(AI_DETECT_ARGS_ST argst){
    if(!g_svp){
        g_svp = make_unique<CSvp>();
        g_svp->m_bDebug=true;
        cout<<"start make csvp"<<endl;
    }
    int nret;
    string sDataDir(argst.data_dir);
    string sModelName(argst.model_name);
    nret = g_svp->DetectInit(argst);
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
//int ai_detect(char *model_name, AI_IMAGE_FRAME_S *pframe, ST_BOX boxes[MAX_BOX_NUM], int*pnbox){    
int ai_detect(char *model_name, AI_IMAGE_S *pframe, ST_BOX boxes[MAX_BOX_NUM], int*pnbox){
    cout<<"34,"<<endl;
    string sModelName(model_name);
    VIDEO_FRAME_INFO_S *pstVFrame = (VIDEO_FRAME_INFO_S *)pframe->frame_info;
    
    HI_U32 u32SizeYolo = (pstVFrame->stVFrame.u64PhyAddr[1] - pstVFrame->stVFrame.u64PhyAddr[0]) * 3 / 2;
    HI_U64 pVirAddrYolo = (HI_U64) (HI_MPI_SYS_Mmap(pstVFrame->stVFrame.u64PhyAddr[0], u32SizeYolo));
    pstVFrame->stVFrame.u64VirAddr[0] = pVirAddrYolo;
    pstVFrame->stVFrame.u64VirAddr[1] = pVirAddrYolo + (pstVFrame->stVFrame.u64PhyAddr[1] - pstVFrame->stVFrame.u64PhyAddr[0]);
    pstVFrame->stVFrame.u64VirAddr[2] = pVirAddrYolo + (pstVFrame->stVFrame.u64PhyAddr[1] - pstVFrame->stVFrame.u64PhyAddr[0]);
    pstVFrame->stVFrame.u32Stride[0] = pstVFrame->stVFrame.u32Stride[0];
    pstVFrame->stVFrame.u32Stride[1] = pstVFrame->stVFrame.u32Stride[1];
    pstVFrame->stVFrame.u32Stride[2] = pstVFrame->stVFrame.u32Stride[2];

    vector<ST_BOX> vBox;
    int nret = g_svp->Detect(sModelName, pstVFrame, vBox);
    HI_MPI_SYS_Munmap((void *)pstVFrame->stVFrame.u64VirAddr[0], u32SizeYolo);
    cout<<"39,";
    *pnbox = min(MAX_BOX_NUM, (int)vBox.size());
    for(int i = 0; i< *pnbox; i++){
        boxes[i] = vBox[i];
    }
    return nret;
}
