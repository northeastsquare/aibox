#include <memory>
#include "opencv2/imgproc/types_c.h"
#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>

#include "GeneralCls.h"
#include "YoloCls.h"
#include "ai_global.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "aibox.h"
#include "ai_npu_sdk.h"

#ifdef __cplusplus
}
#endif


#include <sstream>
#include <fstream>
#include <memory>
//#include "CSvp.h"
#include "YoloCls.h"
#include "ModelHandleManager.hpp"


using namespace std;


static  shared_ptr<CModelHandleManager> g_cModelHandleManager;
bool m_bDebug = true;

int ai_model_init(AI_MODEL_TYPE_E model_type, char* model_file, int net_width, int net_height, int class_number \
        , int npu_core_id, AI_MODEL_HANDLE* model_handle){
    if(!g_cModelHandleManager){
        g_cModelHandleManager = make_shared<CModelHandleManager>(m_bDebug);
    }
    if (model_type == AI_MODEL_TYPE_YOLOV3){
        string sModelPath(model_file);
        if(npu_core_id<0 || npu_core_id>= SVP_NNIE_ID_BUTT){
            ostringstream oss;
            oss<<"nnie core id"<< npu_core_id <<" invalid"<<endl;
            runtime_error(oss.str());
        }
        SVP_NNIE_ID_E iNnieCore = (SVP_NNIE_ID_E)npu_core_id;
        std::shared_ptr<YoloCls> pYc = make_shared<YoloCls>(sModelPath, net_width, net_height, class_number, iNnieCore);
        pYc->m_bDebug = m_bDebug;
        g_cModelHandleManager->setModelInst(AI_MODEL_TYPE_YOLOV3, pYc, *model_handle);
    }else{
        cout<<"model type not support "<<(int)model_type<<endl;
        return -1;
    }
    cout<<"handle:"<<*model_handle<<endl;
    return 0;
}

/**
 * 方框检测
 * @param model_name 模型名称
 * @param frame 待分类图片
 * @param inout boxes 检测结果方框列表，需要调用者提前分配存储空间
 * @param 
 * @return 0：成功， 102：npu 储存不足， 103：未初始化
 */
int ai_model_yolov3(AI_MODEL_HANDLE model_handle, AI_IMAGE_FRAME_S* pframe, float confidence_threshhold, float nms_threshhold \
                    , AI_BOX_S** boxes, int* box_length)
{
    cout<<"34,"<<endl;
    AI_MODEL_TYPE_E eModelType;
    std::shared_ptr<void> pModel;
    int nret = g_cModelHandleManager->getModelInst(model_handle, eModelType, pModel);
    if(nret != 0){
        cout<<"模型为初始化"<<model_handle<<endl;
        return -1;
    }
    
    vector<AI_BOX_S> vBox;
    if (eModelType == AI_MODEL_TYPE_YOLOV3){
        std::shared_ptr<YoloCls> pModel = static_pointer_cast<YoloCls>(pModel);
        pModel->m_fConfThresh = confidence_threshhold;
        pModel->m_fNmsThresh = nms_threshhold;
        if(nms_threshhold>0) pModel->m_bNms = true;
        else pModel->m_bNms = false;
        VIDEO_FRAME_INFO_S *pstVFrame = (VIDEO_FRAME_INFO_S *)pframe->data_frame;
        HI_U32 u32SizeVFrame = (pstVFrame->stVFrame.u64PhyAddr[1] - pstVFrame->stVFrame.u64PhyAddr[0]) * 3 / 2;
        cout<<"u32SizeVFrame:"<<u32SizeVFrame<<endl;
        // HI_U64 pVirAddrVFrame = (HI_U64) (HI_MPI_SYS_Mmap(pstVFrame->stVFrame.u64PhyAddr[0], u32SizeVFrame));
        // pstVFrame->stVFrame.u64VirAddr[0] = pVirAddrVFrame;
        // pstVFrame->stVFrame.u64VirAddr[1] = pVirAddrVFrame + (pstVFrame->stVFrame.u64PhyAddr[1] - pstVFrame->stVFrame.u64PhyAddr[0]);
        // pstVFrame->stVFrame.u64VirAddr[2] = pVirAddrVFrame + (pstVFrame->stVFrame.u64PhyAddr[1] - pstVFrame->stVFrame.u64PhyAddr[0]);
        // pstVFrame->stVFrame.u32Stride[0] = pstVFrame->stVFrame.u32Stride[0];
        // pstVFrame->stVFrame.u32Stride[1] = pstVFrame->stVFrame.u32Stride[1];
        // pstVFrame->stVFrame.u32Stride[2] = pstVFrame->stVFrame.u32Stride[2];
        int imageW = pstVFrame->stVFrame.u32Width;
        int imageH = pstVFrame->stVFrame.u32Height;
        VIDEO_FRAME_INFO_S *pstVFYolo;
        int m_chYoloResize = 65;
        VB_POOL m_vpYoloResize = 0;
        if(imageW != pModel->m_nNetWidth  || imageH != pModel->m_nNetHeight){
            static bool m_bCreatedYoloResizeChn = false;
            
            if(!m_bCreatedYoloResizeChn){
                AI_VPSS_CreateChannel_AP(m_chYoloResize, AI_VENC_CHN_MAX_WIDTH, AI_VENC_CHN_MAX_HEIGHT, pModel->m_nNetWidth, pModel->m_nNetHeight, &m_vpYoloResize, ASPECT_RATIO_NONE);//resize
                m_bCreatedYoloResizeChn = true;
            }
            cout<<"send52"<<endl;
            int hbret = AI_VPSS_SendFrame(m_chYoloResize, pstVFrame);
            if (hbret){
                cout<<"351 return "<<hbret<<","<<m_chYoloResize<<endl;
                return -1;
            }
            cout<<"get"<<endl;
            pstVFYolo = (VIDEO_FRAME_INFO_S *)(malloc(sizeof(VIDEO_FRAME_INFO_S)));
            //std::cout<<"call AI_VPSS_GetChnFrame"<<std::endl;
            hbret = AI_VPSS_GetChnFrame(m_chYoloResize, pstVFYolo);
            if (hbret){
                cout<<"360 return "<<hbret<<","<<m_chYoloResize<<endl;
                return -1;
            }
        }
        else{
            pstVFYolo = pstVFrame;
        }

        HI_U32 u32SizeYolo = (pstVFYolo->stVFrame.u64PhyAddr[1] - pstVFYolo->stVFrame.u64PhyAddr[0]) * 3 / 2;
        HI_U64 pVirAddrYolo = (HI_U64) (HI_MPI_SYS_Mmap(pstVFYolo->stVFrame.u64PhyAddr[0], u32SizeYolo));
        pstVFYolo->stVFrame.u64VirAddr[0] = pVirAddrYolo;
        pstVFYolo->stVFrame.u64VirAddr[1] = pVirAddrYolo + (pstVFYolo->stVFrame.u64PhyAddr[1] - pstVFYolo->stVFrame.u64PhyAddr[0]);
        pstVFYolo->stVFrame.u64VirAddr[2] = pVirAddrYolo + (pstVFYolo->stVFrame.u64PhyAddr[1] - pstVFYolo->stVFrame.u64PhyAddr[0]);
        pstVFYolo->stVFrame.u32Stride[0] = pstVFYolo->stVFrame.u32Stride[0];
        pstVFYolo->stVFrame.u32Stride[1] = pstVFYolo->stVFrame.u32Stride[1];
        pstVFYolo->stVFrame.u32Stride[2] = pstVFYolo->stVFrame.u32Stride[2];

        std::vector<nnie::objInfo> res;
        
        pModel->detectHiVIDEO_FRAME(pstVFYolo, res);
        AI_BOX_S *pboxes = new AI_BOX_S[res.size()];
        for(int i=0; i<res.size(); i++){
            AI_BOX_S box;
            auto oi = res[i];
            box.class_id = oi.cls_id;
            box.confidence = oi.confidence;
            box.x = oi.box.x;
            box.y = oi.box.y;
            box.w = oi.box.width;
            box.h = oi.box.height;
            pboxes[i] = box;
        }
        boxes = &pboxes;
        *box_length = res.size();
        if(m_bDebug){
            cv::Mat mBGR;
            cv::Mat mYVU0(pModel->m_nNetWidth*3/2, pModel->m_nNetHeight, CV_8UC1, (unsigned char *)pVirAddrYolo);
            cv::cvtColor(mYVU0, mBGR, CV_YUV420sp2BGR, 3);
            for(auto oi:res){
                int cid = oi.cls_id;
                cv::Rect2d box = oi.box;
                box.x *= pModel->m_nNetWidth;
                box.y *= pModel->m_nNetHeight;
                box.width *= pModel->m_nNetWidth;
                box.height *= pModel->m_nNetHeight;
                cv::rectangle(mBGR, box, cv::Scalar(255, 255, 255));
                ostringstream oss;
                oss<<cid<<","<<oi.confidence;
                int x = box.x;
                int y = box.y;
                cv::putText(mBGR, oss.str(), cv::Point2d(x, y), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0,0,255), 2);
            }
            static int m_nFrame = 0;
            ostringstream ossfn;
            ossfn<<pModel->m_sDrawDir<<"yolo_"<<m_nFrame<<".jpg"<<endl;
            cv::imwrite(ossfn.str(), mBGR);
            m_nFrame++;
        }

        HI_MPI_SYS_Munmap((void *)pstVFYolo->stVFrame.u64VirAddr[0], u32SizeYolo);
        if(imageW != pModel->m_nNetWidth || imageH != pModel->m_nNetHeight){
            HI_MPI_VPSS_ReleaseChnFrame(m_chYoloResize, 0, pstVFYolo);
            free(pstVFYolo);
        }
    }
    return 0;

}

int ai_model_deinit(AI_MODEL_HANDLE model_handle){
    // AI_MODEL_TYPE_E eModelType;
    // std::shared_ptr<void> pModel;
    // int nret = g_cModelHandleManager.getModelInst(model_handle, eModelType, ptr);
    // if(nret != 0){
    //     cout<<"模型为初始化"<<model_handle<<endl;
    //     return -1;
    // }
    // if (eModelType == AI_MODEL_TYPE_YOLOV3){
    //     std::shared_ptr<YoloCls> pModel = static_pointer_cast<YoloCls>(pModel);
    //     delete pModel;
    // }
    //因为使用了只能指针，map元素删除会，会自动释放yolov3
    g_cModelHandleManager->delModelInst(model_handle);
    return 0;
}


int ai_model_delete(void* p){
    delete []p;
}