//
// Created by surui on 2020/7/1.
//

#ifndef _YOLO_CLS_H_
#define _YOLO_CLS_H_

#include <iostream>
#include <Net.hpp>
#include <unordered_map>
#include <opencv2/opencv.hpp>

class YoloCls
{

public:

    YoloCls(const AI_DETECT_ARGS_ST &st, const std::string &sModelPath);

    ~YoloCls();

    // input absolute path of image
    //void detect(const std::string &imgPath, nnie::clsRes &clsInfo);
    void detect(const std::string &imgPath, std::vector<nnie::objInfo> &detection_results);
	int detectHiVIDEO_FRAME(VIDEO_FRAME_INFO_S *pstVFrame, std::vector<nnie::objInfo> &detection_results);
    // input cv::Mat
    int detectMat(cv::Mat &img, std::vector<nnie::objInfo> &detection_results);
    void decodeResult(std::vector<nnie::objInfo> &detection_results);

    unsigned char * mat2bgr(cv::Mat &img);
    bool m_bDebug;
private:
    int kBoxPerCell = 3;
    int feature_index0 = 0;
    int feature_index1 = 1;
    int feature_index2 = 2;
    int m_iImageWidth;
    int m_iImageHeight;
    int m_nClass;
    float m_fConfThresh;
    float m_fNmsThresh;
    bool m_bNms;
    const std::vector<std::vector<cv::Size2f>> anchors = {
        {{116, 90}, {156, 198}, {373, 326}},
        {{30, 61}, {62, 45}, {59, 119}},
        {{10, 13}, {16, 30}, {33, 23}}};

    //nnie::gParams params;

    nnie::Net net;

private:
    /**
        * Check the valid of parameters.
        * @param gparams
        * @return
        */
    bool validateGparams(nnie::gParams gparams);


};


#endif //NNIE_LITE_GENERALCLS_H
