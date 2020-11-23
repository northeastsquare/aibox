#include "FaceDetector.h"

CFaceDetector::~CFaceDetector(){
    net.clear();
}
void CFaceDetector::init(const std::string &modelPath)
{
    params.modelPath = modelPath;
    params.batchSize = 1;
    params.resizedHeight = 416;
    params.resizedWidth = 416;
    params.inputC = 3;

    if (!validateGparams(params))
    {
        perror("[ERROR] Check your gparams !\n\n");
    }

    net.load_model(params.modelPath.c_str());
}
bool CFaceDetector::validateGparams(nnie::gParams gparams)
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

    // if (gparams.classNum < 1)
    // {
    //     perror("[ERROR] You have to assign the class num !\n\n");
    //     return false;
    // }
    if (gparams.modelPath.empty())
    {
        perror("[ERROR] You have to assign the engine path !\n\n");
        return false;
    }
    return true;
}


//输入图片是指针格式，尺寸用户自己根据网络定义好
bool CFaceDetector::HaveFace(unsigned char * pimg, float thresh)
{
    //std::cerr<<"51\n"<<std::endl;
    net.run(pimg);
    //std::cerr<<"53\n"<<std::endl;
    nnie::Mat feat6, feat7, feat8;
    net.extract(6, feat6);
    net.extract(7, feat7);
    net.extract(8, feat8);

#ifdef __DEBUG__
    // printf("Tensor n : %d\n", feat6.n);
    // printf("Tensor c : %d\n", feat6.channel);
    // printf("\n\nTensor h : %d\n", feat6.height);
    // printf("Tensor w : %d\n", feat6.width);
    // printf("Tensor n : %d\n", feat7.n);
    // printf("Tensor c : %d\n", feat7.channel);
    // printf("\n\nTensor h : %d\n", feat7.height);
    // printf("Tensor w : %d\n", feat7.width);
    // printf("Tensor n : %d\n", feat8.n);
    // printf("Tensor c : %d\n", feat8.channel);
    // printf("\n\nTensor h : %d\n", feat8.height);
    // printf("Tensor w : %d\n", feat8.width);
#endif

    nnie::Mat feats[3]={feat6, feat7, feat8};
    //std::cout<<"start decode"<<std::endl;
    bool haveface = decodeFace(feats, thresh);
    //std::cout<<"endhaveface:"<<haveface<<std::endl;
    return haveface;

}
//输入图片是指针格式，尺寸用户自己根据网络定义好
bool CFaceDetector::HaveFace(cv::Mat input_img, float thresh)
{
    if (input_img.rows != params.resizedHeight || input_img.cols != params.resizedWidth)
        cv::resize(input_img, input_img, cv::Size(params.resizedWidth, params.resizedHeight));
    std::cerr<<"85\n"<<std::endl;
    nnie::Mat in;
    nnie::resize_bilinear(input_img, in, params.resizedWidth, params.resizedHeight, params.inputC);

    // std::cerr<<"166\n"<<std::endl;
    net.run(in.im);
    // std::cerr<<"168\n"<<std::endl;
    nnie::Mat feat6, feat7, feat8;
    net.extract(6, feat6);
    net.extract(7, feat7);
    net.extract(8, feat8);

#ifdef __DEBUG__
    printf("Tensor n : %d\n", feat6.n);
    printf("Tensor c : %d\n", feat6.channel);
    printf("\n\nTensor h : %d\n", feat6.height);
    printf("Tensor w : %d\n", feat6.width);
    printf("Tensor n : %d\n", feat7.n);
    printf("Tensor c : %d\n", feat7.channel);
    printf("\n\nTensor h : %d\n", feat7.height);
    printf("Tensor w : %d\n", feat7.width);
    printf("Tensor n : %d\n", feat8.n);
    printf("Tensor c : %d\n", feat8.channel);
    printf("\n\nTensor h : %d\n", feat8.height);
    printf("Tensor w : %d\n", feat8.width);
#endif
    nnie::Mat feats[3]={feat6, feat7, feat8};
    bool haveface = decodeFace(feats, thresh);
    return haveface;
}
bool CFaceDetector::decodeFace(nnie::Mat feats[], float thresh){
    int cnt =0;
    int proposal_size = 0;
    for(int ifeat = 0; ifeat< 3; ifeat++){
        float *cls = feats[ifeat].data;
        float score_t=0.0;
        int anchor_num = feats[ifeat].channel/2;
        //std::cout<<"anchornum:"<<anchor_num<<std::endl;
        int cls_h = feats[ifeat].height;
        int cls_w = feats[ifeat].width;
        for (int c = 0; c < anchor_num; ++c)
        {
            int c_pt = (c + anchor_num) * cls_h * cls_w;
            for (int h = 0; h < cls_h; ++h)
            {
                int h_pt = c_pt + cls_w * h;
                for (int w = 0; w < cls_w; ++w)
                {
                    score_t = *(cls + (h_pt + w));
                    //std::cout<<"score:"<<score_t<<std::endl;
                    if (score_t >= thresh)
                    {
                        cnt++;
                        //std::cout<<"score:"<<ifeat<<","<<score_t<<",c:"<<c<<",h:"<<h<<"w:"<<w<<std::endl;
                        proposal_size ++;
                    }
                }
            }
        }
    }
    if(cnt>0){
        return true;
    }
    return false;
}
