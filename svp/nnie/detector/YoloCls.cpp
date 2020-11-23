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
#include "YoloCls.h"
#include "hi_comm_video.h"

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

YoloCls::YoloCls(const std::string &modelPath, float fConfThresh, bool bNms, float fNmsThresh);
{
    this->net.load_model(modelPath.c_str());
    this->conf_threshold = fConfThresh;
    this->do_nms = bNms;
    this->nms_threshold = fNmsThresh;
}
YoloCls::~YoloCls(){}


void YoloCls::init(const std::string &modelPath)
{
    params.modelPath = modelPath;
    // You can reference your prototxt to fill these field.
    params.batchSize = 1;
    params.resizedHeight = 544;
    params.resizedWidth = 960;
    params.inputC = 3;
    params.classNum = 2;

    if (!validateGparams(params))
    {
        perror("[ERROR] Check your gparams !\n\n");
    }
    net.load_model(params.modelPath.c_str());
}

bool YoloCls::validateGparams(nnie::gParams gparams)
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
int YoloCls::detectHiVIDEO_FRAME(VIDEO_FRAME_INFO_S *pstVFrame, int orig_w, int orig_h, std::vector<nnie::objInfo> &detection_results)
{
	int resizedHeight = pstVFrame->stVFrame.u32Height;
	int resizedWidth = pstVFrame->stVFrame.u32Width;
	//std::cerr << "image stdetect:" << orig_h << " resizeh:" << params.resizedHeight << " origw:" << orig_w << " resizew:" << params.resizedWidth << std::endl;
	if (resizedHeight != params.resizedHeight || resizedWidth != params.resizedWidth)
	{
		std::cerr << "detect image h,w not equal to net's"<<std::endl;
		return -1;
	}
	unsigned char* data = (unsigned char*)pstVFrame->stVFrame.u64VirAddr[0];
	struct timeval tv1;
	struct timeval tv2;
	long t1, t2, time;

	gettimeofday(&tv1, NULL);

	net.run(data);

	gettimeofday(&tv2, NULL);
	t1 = tv2.tv_sec - tv1.tv_sec;
	t2 = tv2.tv_usec - tv1.tv_usec;
	time = (long)(t1 * 1000 + t2 / 1000);
	//printf("yolov3 ST NNIE  inference time : %dms\n", time);

	gettimeofday(&tv1, NULL);
	decodeResult(orig_w, orig_h, detection_results);
    
	return 0;
}

int YoloCls::detectMat(cv::Mat &img, std::vector<nnie::objInfo> &detection_results)
{
	int orig_h = img.rows;
	int orig_w = img.cols;
	//std::cerr << "detect:" << orig_h << " resizeh:" << params.resizedHeight << " origw:" << orig_w << " resizew:" << params.resizedWidth << std::endl;
	if (orig_h != params.resizedHeight || orig_w != params.resizedWidth)
		cv::resize(img, img, cv::Size(params.resizedWidth, params.resizedHeight));
	unsigned char* data = mat2bgr(img);
	struct timeval tv1;
	struct timeval tv2;
	long t1, t2, time;

	gettimeofday(&tv1, NULL);

	net.run(data);

	gettimeofday(&tv2, NULL);
	t1 = tv2.tv_sec - tv1.tv_sec;
	t2 = tv2.tv_usec - tv1.tv_usec;
	time = (long)(t1 * 1000 + t2 / 1000);
	//printf("yolov3 NNIE inference time : %dms\n", time);

	gettimeofday(&tv2, NULL);
    t1 = tv2.tv_sec - tv1.tv_sec;
    t2 = tv2.tv_usec - tv1.tv_usec;
    time = (long) (t1 * 1000 + t2 / 1000);
    //printf("yolov3 postProcess : %dms\n", time);
    //printf("yolov3 finish\n");


	decodeResult(orig_w, orig_h, detection_results);

	return 0;
}

void YoloCls::decodeResult(int orig_image_w, int orig_image_h, std::vector<nnie::objInfo> &detection_results)
{
    nnie::Mat output0 = net.getOutputTensor(0);
    nnie::Mat output1 = net.getOutputTensor(1);
    nnie::Mat output2 = net.getOutputTensor(2);

    /*yolov3的参数*/
    std::vector<int> ids;
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;

    parseYolov3Feature(params.resizedWidth,
                       params.resizedHeight,
                       params.classNum,
                       kBoxPerCell,
                       feature_index0,
                       conf_threshold,
                       anchors[0],
                       output0,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(params.resizedWidth,
                       params.resizedHeight,
                       params.classNum,
                       kBoxPerCell,
                       feature_index1,
                       conf_threshold,
                       anchors[1],
                       output1,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(params.resizedWidth,
                       params.resizedHeight,
                       params.classNum,
                       kBoxPerCell,
                       feature_index2,
                       conf_threshold,
                       anchors[2],
                       output2,
                       ids,
                       boxes,
                       confidences);

    std::vector<int> indices;

    const char *cls_names[] = {"background", "body", "head"};
    //std::vector<nnie::objInfo> detection_results;

    if (is_nms)
    {
        cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, indices);
    } else
    {
        for (int i = 0; i < boxes.size(); ++i)
        {
            indices.push_back(i);
        }
    }
    //cv::Mat mDraw = mBGR.clone();
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        // remap box in src input size.
        auto remap_box = RemapBoxOnSrc(cv::Rect2d(box), params.resizedWidth, params.resizedHeight);
        nnie::objInfo object_detection;
        object_detection.box = remap_box;
        object_detection.cls_id = ids[idx] + 1;
        object_detection.confidence = confidences[idx];
        

        float xmin = object_detection.box.x;
        float ymin = object_detection.box.y;
        float xmax = object_detection.box.x + object_detection.box.width;
        float ymax = object_detection.box.y + object_detection.box.height;
        
        if (orig_image_h != params.resizedHeight || orig_image_w != params.resizedWidth){
            xmin = int(xmin/params.resizedWidth*orig_image_w);
            ymin = int(ymin/params.resizedHeight*orig_image_h);
            double w = int(object_detection.box.width/params.resizedWidth*orig_image_w);
            double h = int(object_detection.box.height/params.resizedHeight*orig_image_h);
            object_detection.box.x = xmin;
            object_detection.box.y = ymin;
            object_detection.box.width = w;
            object_detection.box.height = h;
        }
        detection_results.push_back(std::move(object_detection));
        float confidence = object_detection.confidence;
        int cls_id = object_detection.cls_id;
        //const char *cls_name = cls_names[cls_id];
        //printf("%d %s %.3f %.3f %.3f %.3f %.3f\n", cls_id, cls_name, confidence, object_detection.box.x, object_detection.box.y, object_detection.box.width, object_detection.box.height);
    }


}

void YoloCls::detect(const std::string &imgPath, std::vector<nnie::objInfo> &detection_results)
{
    cv::Mat img = cv::imread(imgPath.c_str());
    detectMat(img, detection_results);
}