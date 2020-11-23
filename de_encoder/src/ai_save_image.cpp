#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hi_math.h>

// #include "hisi_open.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"


#include "ai_save_image.h"

/************************** C FUNC **************************/
#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus


#include <vector>
using namespace std;
int ai_save_image(char *file_path, AI_IMAGE_S *image)
{
    if(!file_path || !image)
        return -1;
    char *nv21_addr = (char *)(image->vir_addr[0]);
    int nv21_width = image->width;
    int nv21_height = image->height;
    printf("save1 %d,%d\r\n",nv21_width,nv21_height);
    //char * nv21_addr = new char[nv21_width*nv21_height*3/2];
    //memcpy(nv21_addr,image->vir_addr[0],nv21_height*nv21_width);
    //memcpy(nv21_addr+ nv21_height*nv21_width,image->vir_addr[1],nv21_height*nv21_width/2);
   /* cv::Rect rect(0, 0, nv21_width, nv21_height);

    if(!nv21_addr || nv21_width <= 0 || nv21_height <= 0)
        return -1;

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    nv21_height = ALIGN_UP(nv21_height, 2);
    nv21_width =  ALIGN_UP(nv21_width, 16);*/

    cv::Mat image_nv21(nv21_height + nv21_height / 2, nv21_width, CV_8UC1, (unsigned char *)nv21_addr);
    cv::Mat image_bgr;
   // cv::Mat image_bgr_roi;
   printf("save2\r\n");
    cvtColor(image_nv21, image_bgr, cv::COLOR_YUV2BGR_NV21);
    printf("save3\r\n");
    if(image_bgr.empty())
    {
        printf("image_bgr empty \n");
        return -1;
    }

    bool res = imwrite(file_path, image_bgr);
    printf("save4,res %d\r\n",res);
    return 0;
}

void extent_face_rect(MGVL1_RECT_S *rect, float scale, int frame_width, int frame_height)
{
    int face_w = 0, face_h = 0;
    int center_x = 0, center_y = 0;

    if(!rect || scale < 0)
    {
        printf("input param err !!!\n");
        return;
    }

    center_x = (rect->right + rect->left) / 2;
    center_y = (rect->top + rect->bottom) / 2;

    center_x &= ~3;     //@ 4�ı���
    center_y &= ~3;

    face_w = rect->right - rect->left;
    face_h = rect->bottom - rect->top;

    face_w *= 1 + scale;
    face_h *= 1 + scale;

    if(face_w & 0x1f)   //@ ȡ32����
        face_w = (face_w & ~0x1f) + 32;
    if(face_h & 0x1f)   //@ ȡ32����
        face_h = (face_h & ~0x1f) + 32;

    if(face_w < 64)
        face_w = 64;
    if(face_h < 64)
        face_h = 64;

    if(face_w > frame_width)
    {
        face_w = (frame_width & ~0x1f);
    }

    if(face_h > frame_height)
    {
        face_h = (frame_height & ~0x1f);
    }

    if(center_x + face_w / 2 > frame_width)
    {
        rect->right = frame_width;
        rect->left = rect->right - face_w;
    }
    else if(center_x - face_w / 2 < 0)
    {
        rect->left = 0;
        rect->right = rect->left + face_w;
    }
    else
    {
        rect->left = center_x - face_w / 2;
        rect->right = rect->left + face_w;
    }

    if(center_y + face_h / 2 > frame_height)
    {
        rect->bottom = frame_height;
        rect->top = rect->bottom - face_h;
    }
    else if(center_y - face_h / 2 < 0)
    {
        rect->top = 0;
        rect->bottom = rect->top + face_h;
    }
    else
    {
        rect->top = center_y - face_h / 2;
        rect->bottom = rect->top + face_h;
    }
}

int ai_save_frame(char *file_path,MGVL1_FRAME_S *image)
{
    cv::Mat image_bgr,image_rect;
    bool bSaveRect=false;
    MGVL1_RECT_S rect;
    if(!file_path)
    {
        printf("file path not exist");
        return -1;
    }

    //实际大小
    int height=image->height,width=image->width;
    height = ALIGN_UP(height, 2);
    width =  ALIGN_UP(width, 16);

    if(!image || height <= 0 || width <= 0)
        return -1;

    if (MGVL1_FRAME_SOURCE_DATA==image->addr_type)
    {
        cv::Mat image_nv21(height + height / 2, width, CV_8UC1, (unsigned char *)image->data);

        cvtColor(image_nv21, image_bgr, cv::COLOR_YUV2BGR_NV21);

    }
    else if(MGVL1_FRAME_SOURCE_ADDR==image->addr_type)
    {
        //获取数据
        char * pImageData=(char *)(image->addr.vir_addr[0]);
        cv::Mat image_nv21(height + height / 2, width, CV_8UC1, (unsigned char *)pImageData);
        cvtColor(image_nv21, image_bgr, cv::COLOR_YUV2BGR_NV21);

        if(image_bgr.empty())
        {
            printf("image_bgr empty \n");
            return -1;
        }

        if ((image->rect.left!=image->rect.right)&&(image->rect.top!=image->rect.bottom))
        {
            bSaveRect=true;
            memcpy(&rect,&image->rect,sizeof(MGVL1_RECT_S));

            extent_face_rect(&rect, 0.0, width, height);
            image_rect = image_bgr(cv::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top));
        }


    }
    else
    {
        printf("unkown source data type");
        return -1;
    }


    if (bSaveRect)
    {
        if(image_rect.empty())
        {
            printf("image_rect empty \n");
            return -1;
        }

        imwrite(file_path, image_rect);
    }
    else
    {
        imwrite(file_path, image_bgr);

    }

    return 0;

}

#ifdef __cplusplus
}
#endif //@__cplusplus
