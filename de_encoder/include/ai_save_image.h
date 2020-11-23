/*
 *  @file: ai_save_image.h
 *  @brief:
 *  @version: 0.0
 *  @author:
 *  @date: 2020/07/08
 */
/******************************************************************************
@note
    Copyright 2017, Megvii Corporation, Limited
                            ALL RIGHTS RESERVED
******************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ai_common.h"
// #include "hisi_open.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* frame addr
*/
typedef struct MGVL1_FRAME_ADDR_ST
{
    HI_U32  stride[3];                    // Y U V stride
    void*     phy_addr[3];
    void*     vir_addr[3];                  // Y U V������ָ��
} MGVL1_FRAME_ADDR_S;

/**
 * frame data source type
 */
typedef enum MGVL1_FRAME_SOURCE_TYPE_ENUM
{
    MGVL1_FRAME_SOURCE_ADDR = 0x00,
    MGVL1_FRAME_SOURCE_DATA = 0x01,
    MGVL1_FRAME_SOURCE_MAX,
} MGVL1_FRAME_SOURCE_TYPE_E;

/**
 * supported image type
 */
typedef enum MGVL1_IMAGE_TYPE_ENUM
{
    MGVL1_IMAGE_NV21 = 0,
    MGVL1_IMAGE_GRAY,
    MGVL1_IMAGE_BGR,
} MGVL1_IMAGE_TYPE_E;

/**
 * supported image rotate degree
 */
typedef enum MGVL1_IMAGE_ROTATION_ENUM
{
    MGVL1_ROTATION_DEG0   = 0,
    MGVL1_ROTATION_DEG90  = 90,
    MGVL1_ROTATION_DEG180 = 180,
    MGVL1_ROTATION_DEG270 = 270,
    MGVL1_ROTATION_MAX,
} MGVL1_IMAGE_ROTATION_E;

/**
 * rect info
 */
typedef struct MGVL1_RECT_ST
{
    int left;
    int top;
    int right;
    int bottom;
} MGVL1_RECT_S;

/**
* 4 point rect info
**/
typedef struct MGVL1_4POINT_RECT_ST
{
    int x1;//���Ͻ�
    int y1;
    int x2;//���Ͻ�
    int y2;
    int x3;//���½�
    int y3;
    int x4;//���½�
    int y4;
}MGVL1_4POINT_RECT_S;
/**
 *  frame structure from the camera used by SDK to detect
 */
typedef struct MGVL1_FRAME_ST
{
    MGVL1_FRAME_SOURCE_TYPE_E       addr_type; //֡��ַ����
    union
    {
        MGVL1_FRAME_ADDR_S          addr;
        unsigned char*              data;
    };
    int                             width;      //֡��
    int                             height;     //֡��
    MGVL1_IMAGE_ROTATION_E  		rotation;	//Ŀǰδʹ��
    MGVL1_IMAGE_TYPE_E      		type;     //֡�ĸ�ʽ
    HI_U64                        frame_id;   // ֡����ţ����������ʾ����������Ƶ֡��seq
    HI_U64                        track_id;   // track_id,�����Ե�ʱ����
    HI_U64						frame_pts;	// ֡��ʱ�����Ŀǰδ�õ���Ԥ��
    union
    {
        //�����image��ƫ������,�����Ե�ʱ����Ҫ�á�
        MGVL1_RECT_S			    rect; //������/����/������/�ǻ��������Ե�ʱ���á�width��ΧΪ[64-1920],height��ΧΪ[64-1080]
        MGVL1_4POINT_RECT_S         point_rect;//���������Ե�ʱ����,�����������������ʱ��width��ΧΪ[64-1920],height��ΧΪ[64-1080] //@ Ԥ��
    };
    void*                           user_data;      // �û�˽�����ݣ����ڽ���ش�
    int                             reserved[16];   // ����
} MGVL1_FRAME_S;

void extent_face_rect(MGVL1_RECT_S *rect, float scale, int frame_width, int frame_height);

int ai_save_frame(char *file_path,MGVL1_FRAME_S *image);

/**
* @fn          ai_save_image
* @brief       把AI_IMAGE_S结构体保存成图像
* @param[in]   file_path 图像文件名
               image   AI_IMAGE_S结构体
* @param[out]  
* @return       0 success, -1 fail
*/
int ai_save_image(char *file_path, AI_IMAGE_S *image);



#ifdef __cplusplus
}
#endif
