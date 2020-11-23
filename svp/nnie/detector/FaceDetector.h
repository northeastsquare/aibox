#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H


#include <Net.hpp>
#include <unordered_map>
#include <opencv2/opencv.hpp>

class CFaceDetector
{

public:

    CFaceDetector()
    {};

    CFaceDetector(const std::string &modelPath);

    ~CFaceDetector();

    void init(const std::string &modelPath);
    bool HaveFace(unsigned char * pimg, float thresh);
    bool HaveFace(cv::Mat input_img, float thresh);
    bool decodeFace(nnie::Mat feats[], float thresh);

private:

    nnie::gParams params;

    nnie::Net net;

private:
    /**
        * Check the valid of parameters.
        * @param gparams
        * @return
        */
    bool validateGparams(nnie::gParams gparams);


    /**
     * The function is used to parse output tensor from NNIE.
     * clsIdxMask is a one channel matrix which each pixel represent a classification.
     * colorMap is a three channel matrix, it's a rgb picture for visualization.
     *
     * @param outTensor
     * @param clsIdxMask
     * @param colorMap
     */
    void parseTensor(nnie::Mat outTensor, cv::Mat clsIdxMask, cv::Mat &colorMask);
};


#endif //FACE_DETECTOR_H
