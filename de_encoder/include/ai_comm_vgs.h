#ifndef AIBOX_SDK_AI_COMM_VGS_H
#define AIBOX_SDK_AI_COMM_VGS_H

#include "ai_common.h"
#include "loadbmp.h"

#define MAX_VGS_LINES 50
#define MAX_VGS_QUAD 20
//////// 线条粗细、绘制坐标和长宽等空间参数都遵循2对齐原则

typedef struct ai_VGS_Line //单条划线结构体 
{
    POINT_S start_pt;      //线段起始点
    POINT_S end_pt;        //线段终点
    HI_S32 thick;          //线粗
    HI_U8 Red;             //红色参量
    HI_U8 Green;           //绿色参量
    HI_U8 Blue;            //蓝色参量
}AI_VGS_Line;

typedef struct ai_VGS_Lines //线条集合结构体
{
    HI_S32 number;                      //绘制线数
    AI_VGS_Line line[MAX_VGS_LINES];    //线条参数
}AI_VGS_Lines;

typedef struct ai_VGS_Quadrangle   //四边形结构体
{
    POINT_S the_point[4];          //四顶点坐标-顺序0左上，1右上，2左下，3右下
    HI_S32 thick;                  //边框粗细
    HI_U8 Red;                     //红色参量
    HI_U8 Green;                   //绿色参量
    HI_U8 Blue;                    //蓝色参量
    HI_BOOL SOLID;                 //是否实心
}AI_VGS_Quadrangle;

typedef struct ai_VGS_Quadrangles
{
    HI_S32 number;
    AI_VGS_Quadrangle quad[MAX_VGS_QUAD];
}AI_VGS_Quadrangles;

typedef enum VGS_TYPE
{
    DRAW_LINE = 0,
    DRAW_QUAD,
    DRAW_OSD,
    DRAW_ROTATE,
}VGS_DRAW_TYPE;




HI_S32 AI_VGS_COMM(VIDEO_FRAME_INFO_S *frame, HI_S32 type, HI_VOID * Para);

HI_S32 AI_VGS_DRAW_LINES(VIDEO_FRAME_INFO_S *frame, AI_VGS_Lines * para);

HI_S32 AI_VGS_DRAW_QUADS(VIDEO_FRAME_INFO_S *frame, AI_VGS_Quadrangles * para);

HI_S32 AI_VGS_DRAW_ROTATE(VIDEO_FRAME_INFO_S *frame, ROTATION_E * para);

#endif //AIBOX_SDK_AI_COMM_VGS_H