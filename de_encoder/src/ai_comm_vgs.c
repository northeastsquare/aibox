#include "ai_comm_vgs.h"

VGS_TASK_ATTR_S stTask;
AI_VGS_Lines * line_para;
AI_VGS_Quadrangles * quad_para;


HI_S32 AI_VGS_DRAW_LINES(VIDEO_FRAME_INFO_S *frame, AI_VGS_Lines * para)
{
    HI_S32 s32Ret = AI_VGS_COMM(frame,DRAW_LINE,para);
    return s32Ret;
}

HI_S32 AI_VGS_DRAW_QUADS(VIDEO_FRAME_INFO_S *frame, AI_VGS_Quadrangles * para)
{
    HI_S32 s32Ret = AI_VGS_COMM(frame,DRAW_QUAD,para);
    return s32Ret;
}

HI_S32 AI_VGS_DRAW_ROTATE(VIDEO_FRAME_INFO_S *frame, ROTATION_E * para)
{
    HI_S32 s32Ret = AI_VGS_COMM(frame,DRAW_ROTATE,para);
    return s32Ret;
}


HI_S32 AI_VGS_COMM(VIDEO_FRAME_INFO_S *frame, HI_S32 type, HI_VOID * Para)
{
    VGS_TASK_ATTR_S stTask;
    VGS_HANDLE VgsHandle = -1;
    HI_S32 s32Ret = HI_SUCCESS;

    memcpy(&stTask.stImgIn, frame, sizeof(VIDEO_FRAME_INFO_S));
    memcpy(&stTask.stImgOut, frame, sizeof(VIDEO_FRAME_INFO_S));
    s32Ret = HI_MPI_VGS_BeginJob(&VgsHandle);
    if (s32Ret != HI_SUCCESS)
    {
        printf("Vgs begin job fail,Error(%#x)\n", s32Ret);
        return s32Ret;
    }
    else printf("Vgs job begin done\n");

    if (type == DRAW_LINE)
    {
        line_para = (AI_VGS_Lines *)Para;
        if (line_para->number <= 0)
            return -1;
        VGS_DRAW_LINE_S vgs_line[line_para->number];
        
        for (int i = 0; i < line_para->number; i ++)
        {
            vgs_line[i].u32Thick = line_para->line[i].thick;
            if (vgs_line[i].u32Thick %2 !=0)
                vgs_line[i].u32Thick += 1;
            if (vgs_line[i].u32Thick > 8) 
                vgs_line[i].u32Thick = 8;
            else if (vgs_line[i].u32Thick < 2)
                vgs_line[i].u32Thick = 2;
            vgs_line[i].stStartPoint = line_para->line[i].start_pt;
            vgs_line[i].stStartPoint.s32X += vgs_line[i].stStartPoint.s32X%2;
            vgs_line[i].stStartPoint.s32Y += vgs_line[i].stStartPoint.s32Y%2;
            vgs_line[i].stEndPoint = line_para->line[i].end_pt;
            vgs_line[i].stEndPoint.s32X += vgs_line[i].stEndPoint.s32X%2;
            vgs_line[i].stEndPoint.s32Y += vgs_line[i].stEndPoint.s32Y%2;
            vgs_line[i].u32Color = line_para->line[i].Red*256*256 + line_para->line[i].Green*256 + line_para->line[i].Blue;
        }
        s32Ret = HI_MPI_VGS_AddDrawLineTaskArray(VgsHandle,&stTask,vgs_line,line_para->number);
         if (s32Ret != HI_SUCCESS)
        {
            printf("HI_MPI_VGS_AddLineTask fail,Error(%#x)\n", s32Ret);
            HI_MPI_VGS_CancelJob(VgsHandle);
            return s32Ret;
        }
         else printf("Vgs line task add done\n");
    }
    else if (type == DRAW_QUAD)
    {
        quad_para = (AI_VGS_Quadrangles * )Para;
        if (quad_para->number <= 0)
            return -1;
        VGS_ADD_COVER_S vgs_quad[quad_para->number];

        for (int i = 0; i < quad_para->number; i ++)
        {
            vgs_quad[i].enCoverType = COVER_QUAD_RANGLE;
            vgs_quad[i].stQuadRangle.bSolid = quad_para->quad[i].SOLID;
            vgs_quad[i].u32Color = quad_para->quad[i].Red*256*256 + quad_para->quad[i].Green*256 + quad_para->quad[i].Blue;
            vgs_quad[i].stQuadRangle.u32Thick = quad_para->quad[i].thick;
            if (vgs_quad[i].stQuadRangle.u32Thick %2 !=0)
                vgs_quad[i].stQuadRangle.u32Thick += 1;
            if (vgs_quad[i].stQuadRangle.u32Thick > 8) 
                vgs_quad[i].stQuadRangle.u32Thick = 8;
            else if (vgs_quad[i].stQuadRangle.u32Thick < 2)
                vgs_quad[i].stQuadRangle.u32Thick = 2;
            for (int j = 0; j < 4; j ++)
            {
                vgs_quad[i].stQuadRangle.stPoint[j] = quad_para->quad[i].the_point[j]; 
                vgs_quad[i].stQuadRangle.stPoint[j].s32X += quad_para->quad[i].the_point[j].s32X%2;
                vgs_quad[i].stQuadRangle.stPoint[j].s32Y += quad_para->quad[i].the_point[j].s32Y%2; 
            }
        }
        s32Ret = HI_MPI_VGS_AddCoverTaskArray(VgsHandle,&stTask,vgs_quad,quad_para->number);
        if (s32Ret != HI_SUCCESS)
        {
            printf("HI_MPI_VGS_AddCoverTask fail,Error(%#x)\n", s32Ret);
            HI_MPI_VGS_CancelJob(VgsHandle);
            return s32Ret;
        }
         else printf("Vgs cover task add done\n");
    }
    else if (type == DRAW_OSD)
    {
        VGS_ADD_OSD_S osd_para;
    }
    else if (type == DRAW_ROTATE)
    {
        ROTATION_E  * rotate_para = (ROTATION_E * )Para;
        s32Ret = HI_MPI_VGS_AddRotationTask(VgsHandle,&stTask,*rotate_para);
        if (s32Ret != HI_SUCCESS)
        {
            printf("HI_MPI_VGS_Add rotate Task fail rotate %d,Error(%#x)\n", *rotate_para,s32Ret);
            HI_MPI_VGS_CancelJob(VgsHandle);
            return s32Ret;
        }
         else printf("Vgs rotate task add done\n");
    }
    s32Ret = HI_MPI_VGS_EndJob(VgsHandle);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VGS_EndJob fail,Error(%#x)\n", s32Ret);
        HI_MPI_VGS_CancelJob(VgsHandle);
        return s32Ret;
    }
 
    return s32Ret;
}