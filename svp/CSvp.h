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

#ifdef __cplusplus
}
#endif


using namespace std;

class CSvp{
public:
    int DetectInit(const AI_DETECT_ARGS_ST &argst){
        string sModelName(argst.model_name);
        string sDataDir(argst.data_dir);
        if(sModelName == "yolov3" && m_mapInferor.find(sModelName) == m_mapInferor.end()){
            std::string smpath=sDataDir +"/model/yolov3_80cls_yvu_inst.wk";
            m_sDrawDir = sDataDir + "/drawed_images/";
            cout<<"15,"<<endl;
            //make_shared<YoloCls>(argst, smpath);
            //YoloCls *pDetector = new YoloCls(argst, smpath);
            shared_ptr<YoloCls> pDetector = make_shared<YoloCls>(argst, smpath);
            pDetector->m_bDebug=m_bDebug;
            shared_ptr<AI_DETECT_ARGS_ST> pArg = make_shared<AI_DETECT_ARGS_ST>(argst);
            m_mapInferor.emplace(string("yolov3"), make_pair(pArg, pDetector));
            cout<<"20 "<<endl;
            //--------------后面替换----------------------
        }
        return 0;
    }
    int Detect(const string &sModelName, VIDEO_FRAME_INFO_S *pstVFrame, vector<ST_BOX> &boxes){
        cout<<",pstVFrame width："<<pstVFrame->stVFrame.u32Width<<",height:"<<pstVFrame->stVFrame.u32Height<<endl;
        if (m_mapInferor.find(sModelName) == m_mapInferor.end()){
            cout<<"模型"<<sModelName<<"未初始化"<<endl;
            return 103;
        }
        shared_ptr<AI_DETECT_ARGS_ST> pArgs = m_mapInferor[sModelName].first;
        std::vector<nnie::objInfo> res;
        int imageW = pstVFrame->stVFrame.u32Width;
        int imageH = pstVFrame->stVFrame.u32Height;
        if (sModelName == "yolov3"){
            cout<<"40,"<<endl;
            VIDEO_FRAME_INFO_S *pstVFYolo;
            if(imageW != pArgs->image_width || imageH != pArgs->image_height){
                if(!m_bCreatedYoloResizeChn){
                    AI_VPSS_CreateChannel_AP(m_chYoloResize, AI_VENC_CHN_MAX_WIDTH, AI_VENC_CHN_MAX_HEIGHT, pArgs->image_width, pArgs->image_height, &m_vpYoloResize, ASPECT_RATIO_NONE);//resize
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

            cout<<"detect78 "<<endl;
            shared_ptr<YoloCls> pDetector = static_pointer_cast<YoloCls>(m_mapInferor[sModelName].second);
            pDetector->detectHiVIDEO_FRAME(pstVFYolo, res);
            for(auto oi:res){
                ST_BOX box;
                box.clss_id = oi.cls_id;
                box.conf = oi.confidence;
                box.x = oi.box.x;
                box.y = oi.box.y;
                box.w = oi.box.width;
                box.h = oi.box.height;
                boxes.push_back(box);
            }
            if(m_bDebug){
                cv::Mat mBGR;
                cv::Mat mYVU0(pArgs->image_width*3/2, pArgs->image_height, CV_8UC1, (unsigned char *)pVirAddrYolo);
                cv::cvtColor(mYVU0, mBGR, CV_YUV420sp2BGR, 3);
                for(auto oi:res){
                    int cid = oi.cls_id;
                    cv::Rect2d box = oi.box;
                    box.x *= pArgs->image_width;
                    box.y *= pArgs->image_height;
                    box.width *= pArgs->image_width;
                    box.height *= pArgs->image_height;
                    cv::rectangle(mBGR, box, cv::Scalar(255, 255, 255));
                    ostringstream oss;
                    oss<<cid<<","<<oi.confidence;
                    int x = box.x;
                    int y = box.y;
                    cv::putText(mBGR, oss.str(), cv::Point2d(x, y), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0,0,255), 2);
                }
                ostringstream ossfn;
                ossfn<<m_sDrawDir<<"yolo_"<<m_nFrame<<".jpg"<<endl;
                cv::imwrite(ossfn.str(), mBGR);
                m_nFrame++;
            }

            HI_MPI_SYS_Munmap((void *)pstVFYolo->stVFrame.u64VirAddr[0], u32SizeYolo);
            if(imageW != pArgs->image_width || imageH != pArgs->image_height){
                HI_MPI_VPSS_ReleaseChnFrame(m_chYoloResize, 0, pstVFYolo);
                free(pstVFYolo);
            }
        }
    }
    bool m_bDebug=true;
private:
    string m_sDrawDir;
    int m_nFrame=0;
    bool m_bCreatedYoloResizeChn = false;
    int m_chYoloResize=65;
    unsigned int m_vpYoloResize=0;
    string m_sDataDir;
    map<string, pair<shared_ptr<AI_DETECT_ARGS_ST>, shared_ptr<void> > > m_mapInferor;
    //map<string, shared_ptr<AI_DETECT_ARGS_ST>> m_mapModelArgs;
    map<int, int> m_mapTest;

};