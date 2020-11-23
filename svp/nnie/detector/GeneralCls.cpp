//
// Created by surui on 2020/7/1.
//
#include <iostream>
#include "GeneralCls.h"


GeneralCls::GeneralCls(const std::string &modelPath)
{
    // params.modelPath = modelPath;
    // // You can reference your prototxt to fill these field.
    // params.batchSize = 1;
    // params.resizedHeight = 28;
    // params.resizedWidth = 28;
    // params.inputC = 1;
    // params.classNum = 10;

    // if (!validateGparams(params))
    // {
    //     perror("[ERROR] Check your gparams !\n\n");
    // }
    // net.load_model(params.modelPath.c_str());
}


void GeneralCls::init(const std::string &modelPath)
{
    params.modelPath = modelPath;
    // You can reference your prototxt to fill these field.
    params.batchSize = 1;
    params.resizedHeight = 256;
    params.resizedWidth = 192;
    params.inputC = 3;
    params.classNum = 6;

    if (!validateGparams(params))
    {
        perror("[ERROR] Check your gparams !\n\n");
    }
    net.load_model(params.modelPath.c_str());
}


bool GeneralCls::validateGparams(nnie::gParams gparams)
{
    if (gparams.resizedHeight < 1 || gparams.resizedWidth < 1 || gparams.inputC < 1)
    {
        perror("[ERROR] You have to assign the resize info and channel !\n\n");
        return false;
    }
    if (gparams.batchSize < 1)
    {
        perror("[ERROR] You have to assign the batch size !\n\n");
        return false;
    }

    if (gparams.classNum < 1)
    {
        perror("[ERROR] You have to assign the class num !\n\n");
        return false;
    }
    if (gparams.modelPath.empty())
    {
        perror("[ERROR] You have to assign the engine path !\n\n");
        return false;
    }
    return true;
}

GeneralCls::~GeneralCls()
{
    net.clear();
}

void GeneralCls::run(const std::string &imgPath, nnie::clsRes &clsInfo)
{
    cv::Mat im = params.inputC == 1 ? cv::imread(imgPath, 0): cv::imread(imgPath);
    run(im, clsInfo);
}

void GeneralCls::run(cv::Mat input_img, nnie::clsRes &clsInfo)
{
    std::cerr<<"83\n"<<std::endl;
    if (input_img.rows != params.resizedHeight || input_img.cols != params.resizedWidth)
        cv::resize(input_img, input_img, cv::Size(params.resizedWidth, params.resizedHeight));
    std::cerr<<"85\n"<<std::endl;
    nnie::Mat in;
    nnie::resize_bilinear(input_img, in, params.resizedWidth, params.resizedHeight, params.inputC);
    std::cerr<<"88\n"<<std::endl;
    net.run(in.im);
    std::cerr<<"90  \n"<<std::endl;
    nnie::Mat logit;
    net.extract(0, logit);

#ifdef __DEBUG__
    printf("\n\nTensor h : %d\n", logit.height);
    printf("Tensor w : %d\n", logit.width);
    printf("Tensor c : %d\n", logit.channel);
    printf("Tensor n : %d\n", logit.n);
#endif

    float *prob = logit.data;
    int maxIdx = 0;
    float maxProb = -1;
    for (int i = 0; i < logit.width; ++i)
    {
#ifdef __DEBUG__
        printf(" %f ", *prob);
#endif
        if (*prob > maxProb)
        {
            clsInfo.prob.push_back(*prob);
            maxProb = *prob;
            maxIdx = i;
        }
        prob++;
    }
    clsInfo.index = maxIdx;
    free(in.im);
}

void GeneralCls::run(unsigned char *input_img, nnie::clsRes &clsInfo)
{
    // std::cerr<<"83\n"<<std::endl;
    net.run(input_img);
    // std::cerr<<"90  \n"<<std::endl;
    nnie::Mat logit;
    net.extract(0, logit);

#ifdef __DEBUG__
    // printf("\n\nTensor h : %d\n", logit.height);
    // printf("Tensor w : %d\n", logit.width);
    // printf("Tensor c : %d\n", logit.channel);
    // printf("Tensor n : %d\n", logit.n);
#endif

    float *prob = logit.data;
    int maxIdx = 0;
    float maxProb = -1;
    for (int i = 0; i < logit.width; ++i)
    {
#ifdef __DEBUG__
        // printf("mouse %d: %f \n", i, *prob);
#endif
        clsInfo.prob.push_back(*prob);
        if (*prob > maxProb)
        {    
            maxProb = *prob;
            maxIdx = i;
        }
        prob++;
    }
    clsInfo.index = maxIdx;
}

void GeneralCls::run_logistic(cv::Mat input_img, std::vector<nnie::LogisticRes> &lres)
{
    //std::cerr<<"124\n"<<std::endl;
    if (input_img.rows != params.resizedHeight || input_img.cols != params.resizedWidth)
        cv::resize(input_img, input_img, cv::Size(params.resizedWidth, params.resizedHeight));
    //std::cerr<<"127\n"<<std::endl;
    nnie::Mat in;
    nnie::resize_bilinear(input_img, in, params.resizedWidth, params.resizedHeight, params.inputC);
    //std::cerr<<"130\n"<<std::endl;
    net.run(in.im);
    //std::cerr<<"132\n"<<std::endl;
    nnie::Mat logit;
    net.extract(0, logit);

#ifdef __DEBUG__
    // printf("\n\nTensor h : %d\n", logit.height);
    // printf("Tensor w : %d\n", logit.width);
    // printf("Tensor c : %d\n", logit.channel);
    // printf("Tensor n : %d\n", logit.n);
#endif

    float *prob = logit.data;
    int maxIdx = 0;
    float maxProb = -1;
    for (int i = 0; i < logit.width; ++i)
    {
#ifdef __DEBUG__
        //printf(" %f ", *prob);
#endif
        nnie::LogisticRes res;
        res.index = i;
        res.prob = *prob;
        lres.push_back(res);
        prob++;
    }
    free(in.im);
}

void GeneralCls::run_logistic(cv::Mat input_img, std::vector<nnie::LogisticRes> &lres, const double thresh)
{
    //std::cerr<<"124\n"<<std::endl;
    if (input_img.rows != params.resizedHeight || input_img.cols != params.resizedWidth)
        cv::resize(input_img, input_img, cv::Size(params.resizedWidth, params.resizedHeight));
    //std::cerr<<"127\n"<<std::endl;
    nnie::Mat in;
    nnie::resize_bilinear(input_img, in, params.resizedWidth, params.resizedHeight, params.inputC);
    //std::cerr<<"130\n"<<std::endl;
    net.run(in.im);
    //std::cerr<<"132\n"<<std::endl;
    nnie::Mat logit;
    net.extract(0, logit);

#ifdef __DEBUG__
    // printf("\n\nTensor h : %d\n", logit.height);
    // printf("Tensor w : %d\n", logit.width);
    // printf("Tensor c : %d\n", logit.channel);
    // printf("Tensor n : %d\n", logit.n);
#endif

    float *prob = logit.data;
    int maxIdx = 0;
    float maxProb = -1;
    for (int i = 0; i < logit.width; ++i)
    {
#ifdef __DEBUG__
        //printf(" %f ", *prob);
#endif
        if (*prob > thresh)
        {
            nnie::LogisticRes res;
            res.index = i;
            res.prob = *prob;
            lres.push_back(res);
        }
        prob++;
    }
    free(in.im);
}

//输入图片是指针格式，尺寸用户自己定义好
void GeneralCls::run_logistic(unsigned char * pimg, std::vector<nnie::LogisticRes> &lres, std::vector<float> threshs)
{
    //std::cout<<"thresh:"<<thresh<<std::endl;
    // std::cerr<<"166\n"<<std::endl;
    net.run(pimg);
    // std::cerr<<"168\n"<<std::endl;
    nnie::Mat logit;
    net.extract(0, logit);
    assert(threshs.size()==logit.width);

#ifdef __DEBUG__
    // printf("\n\nTensor h : %d\n", logit.height);
    // printf("Tensor w : %d\n", logit.width);
    // printf("Tensor c : %d\n", logit.channel);
    // printf("Tensor n : %d\n", logit.n);
#endif

    float *prob = logit.data;
    int maxIdx = 0;
    float maxProb = -1;
    for (int i = 0; i < logit.width; ++i)
    {
#ifdef __DEBUG__
        // printf("prob%d %f ", i, *prob);
#endif
        if (*prob > threshs[i])
        {
            nnie::LogisticRes res;
            res.index = i;
            res.prob = *prob;
            lres.push_back(res);
        }
        prob++;
    }
}



// Example: General image classification
// ========================= main =================================

int generalcls_main_test()
{
    const std::string &pcModelPath = "./data/nnie_model/mnist/inst_mnist_cycle.wk";
    GeneralCls mnist;
    mnist.init(pcModelPath);
    const std::string &pcSrcFile = "./data/nnie_image/mnist/8_102636.jpg";
    nnie::ClsRes cls_res;
    mnist.run(pcSrcFile, cls_res);

    printf("\n class : %d \n", cls_res.index);

}


