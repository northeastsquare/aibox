#include <memory>
#include "GeneralCls.h"
#include "YoloCls.h"
#include "ai_vpu_sdk.h"

using namespace std;

class CSvp{
public:
    int DetectorInit(const string &sDataDir， const string &sModelName, float fConfThresh, bool bNMS, float fNmsThresh){
        if(sModelName == "yolov3"){
            std::string smpath=sDataDir +"/model/yolov3_inst2cls_yvu420sp_960_544_inst.wk";
            shared_ptr<YoloCls> pDetector = make_shared<YoloCls>(smpath, fConfThresh, bNms, fNmsThresh);
            if(pDetector){
                m_mapInferor[sModelName] = pDetector;
            }
            else{
                return -1;
            }
        }
        return 0;
    }
    int Detect(const string &sModelName, VIDEO_FRAME_INFO_S *pstVFrame, vector<ST_BOX> &boxes){
        if (m_mapInferor.find(sModelName) == m_mapInferor.end()){
            cout<<"模型"<<sModelName<<"未初始化"<<endl;
            return 103;
        }
        std::vector<nnie::objInfo> res;
        int imageW = pstVFrame->stVFrame.u32Width;
        int imageH = pstVFrame->stVFrame.u32Height;
        if (sModelName == "yolov3"){
            shared_ptr<YoloCls> pDetector = m_mapInferor[sModelName];
            pDetector->detectHiVIDEO_FRAME(pstVFrame, imageW, imageH, res);
        }
    
    }
private:
    map<string, shared_ptr<void>> m_mapInferor;
    shared_ptr m_pClassifier;
    template<typename T>
    shared_ptr m_pDetector;

}