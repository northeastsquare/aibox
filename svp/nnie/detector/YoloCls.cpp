#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <vector>
#include <sys/ioctl.h>
#include <dirent.h>
#include "Net.hpp"
#include "util.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/dnn.hpp"
#include "ai_global.h"
#include "YoloCls.h"
#include "hi_comm_video.h"

using namespace std;
using namespace cv;

unsigned char * YoloCls::mat2bgr(cv::Mat &img){
    int step = img.step;
    int h = img.rows;
    int w = img.cols;
    int c = img.channels();
    unsigned char *data = (unsigned char *) malloc(sizeof(unsigned char) * h * w * 3);
    //cv::resize(img, img, cv::Size(416, 416));

    printf("h=%d\n", h);
    printf("w=%d\n", w);
    printf("c=%d\n", c);

    unsigned char *data1 = (unsigned char *) img.data;
    int count = 0;
    for (int k = 0; k < c; k++)
    {
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                data[count++] = data1[i * step + j * c + k];
            }
        }
    }
    return data;
}

YoloCls::YoloCls(const string & sModelPath, int nNetWidth, int nNetHeight, int nClassNumber, SVP_NNIE_ID_E iNnieCore)
:m_nNetWidth(nNetWidth),
m_nNetHeight(nNetHeight),
m_nClass(nClassNumber),
m_bDebug(false){
    cout<<"62"<<endl;
    if(sModelPath.size()==0 || nNetWidth<=0 || nNetHeight<=0 || nClassNumber<=0){
        std::runtime_error("YoloCls::YoloCls, invalide params");
    }
    size_t pos = sModelPath.rfind("/");
    string sdir = sModelPath.substr(pos);
    m_sDrawDir = sdir + "/data/model/../drawed_images/";
    cout<<"before load_model"<<endl;
    net.load_model(sModelPath.c_str(), iNnieCore);
    cout<<"68"<<endl;
}

YoloCls::~YoloCls(){}



int YoloCls::detectHiVIDEO_FRAME(VIDEO_FRAME_INFO_S *pstVFrame, std::vector<nnie::objInfo> &detection_results)
{
    if(pstVFrame->stVFrame.u32Width != m_nNetWidth || pstVFrame->stVFrame.u32Height != m_nNetHeight){
        cerr<<"input image size error"<<pstVFrame->stVFrame.u32Width<<"!="<<m_nNetWidth \
                <<","<< pstVFrame->stVFrame.u32Height<<"!="<<m_nNetHeight<<endl;
        return -1;
    }
	struct timeval tv1;
	struct timeval tv2;
	long t1, t2, time;
    if(m_bDebug){
	    gettimeofday(&tv1, NULL);
    }
    cout<<"net.run"<<endl;
	net.run((unsigned char*)(pstVFrame->stVFrame.u64VirAddr[0]), 3 \
                , pstVFrame->stVFrame.u32Width, pstVFrame->stVFrame.u32Height);
    if(m_bDebug){
        gettimeofday(&tv2, NULL);
        t1 = tv2.tv_sec - tv1.tv_sec;
        t2 = tv2.tv_usec - tv1.tv_usec;
        time = (long)(t1 * 1000 + t2 / 1000);
        cout<<"yolov3 inference time:"<<time<<" ms"<<endl;
    }
	decodeResult(detection_results);
	return 0;
}

void YoloCls::decodeResult(std::vector<nnie::objInfo> &detection_results)
{
    nnie::Mat output0 = net.getOutputTensor(0);
    nnie::Mat output1 = net.getOutputTensor(1);
    nnie::Mat output2 = net.getOutputTensor(2);

    /*yolov3的参数*/
    std::vector<int> ids;
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;

    parseYolov3Feature(m_nNetWidth,
                       m_nNetHeight,
                       m_nClass,
                       kBoxPerCell,
                       feature_index0,
                       m_fConfThresh,
                       anchors[0],
                       output0,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(m_nNetWidth,
                       m_nNetHeight,
                       m_nClass,
                       kBoxPerCell,
                       feature_index1,
                       m_fConfThresh,
                       anchors[1],
                       output1,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(m_nNetWidth,
                       m_nNetHeight,
                       m_nClass,
                       kBoxPerCell,
                       feature_index2,
                       m_fConfThresh,
                       anchors[2],
                       output2,
                       ids,
                       boxes,
                       confidences);

    std::vector<int> indices;

    const char *cls_names[] = {"background", "body", "head"};
    //std::vector<nnie::objInfo> detection_results;

    if (m_bNms)
    {
        cv::dnn::NMSBoxes(boxes, confidences, m_fConfThresh, m_fNmsThresh, indices);
    } else
    {
        for (int i = 0; i < boxes.size(); ++i)
        {
            indices.push_back(i);
        }
    }
    //cv::Mat mDraw = mBGR.clone();
    cout<<"indices:"<<indices.size()<<endl;
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        // remap box in src input size.
        auto remap_box = RemapBoxOnSrc(cv::Rect2d(box), m_nNetWidth, m_nNetHeight);
        nnie::objInfo object_detection;
        object_detection.box = remap_box;
        object_detection.cls_id = ids[idx] + 1;
        object_detection.confidence = confidences[idx];

        float xmin = object_detection.box.x;
        float ymin = object_detection.box.y;
        float w = object_detection.box.width;
        float h = object_detection.box.height;
        
        object_detection.box.x = xmin/m_nNetWidth;
        object_detection.box.y = ymin/m_nNetHeight;
        object_detection.box.width = w/m_nNetWidth;
        object_detection.box.height = h/m_nNetHeight;
        detection_results.push_back(std::move(object_detection));
        float confidence = object_detection.confidence;
        int cls_id = object_detection.cls_id;
        if(m_bDebug){
            cout<<"clsid:"<<cls_id<<",conf:"<<confidence<<",x:"<<object_detection.box.x<<",y:"<<object_detection.box.y \
                    <<",w:"<<object_detection.box.width<<",h:"<<object_detection.box.height;
        }
    }
}
