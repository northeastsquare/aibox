#include "ai_comm_ive.h"
#include "opencv2/opencv.hpp"
#include <time.h>
#include "ai_image_mmz.h"
#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus




HI_S32 AI_COMM_IVE_Affine(AI_IMAGE_S * img, AI_IMAGE_S ** img_res,PERSTRANS_PARA trans_attr)
{
	timespec tp;
	int t =  clock_gettime(CLOCK_REALTIME, &tp);
	auto time1 = tp.tv_sec*1000+tp.tv_nsec/1000000;
    HI_S32 ret = do_warp_by_opencv(img,img_res,trans_attr,0);
	t =  clock_gettime(CLOCK_REALTIME, &tp);
	auto time2 = tp.tv_sec*1000+tp.tv_nsec/1000000;
	printf("affine test %ld,%ld ~~ %ld\r\n",time1, time2, time2 - time1);
	
    return 0;
}

HI_S32 AI_COMM_IVE_PerspTrans(AI_IMAGE_S * img, AI_IMAGE_S ** img_res,PERSTRANS_PARA trans_attr)
{
	timespec tp;
	int t =  clock_gettime(CLOCK_REALTIME, &tp);
	auto time1 = tp.tv_sec*1000+tp.tv_nsec/1000000;
    HI_S32 ret = do_warp_by_opencv(img,img_res,trans_attr,1);
	t =  clock_gettime(CLOCK_REALTIME, &tp);
	auto time2 = tp.tv_sec*1000+tp.tv_nsec/1000000;
	printf("perstrans test %ld,%ld ~~ %ld\r\n",time1, time2, time2 - time1);

    return 0;
}


int do_warp_by_opencv(AI_IMAGE_S * img, AI_IMAGE_S ** img_res,PERSTRANS_PARA trans_attr,int type)
{
	cv::Point2f  src_pt[4],dst_pt[4];
	cv::Point2f  src_pt_uv[4],dst_pt_uv[4];
    if (trans_attr.points_number < 4 && type == 1)
    {
        printf("%d points too few to pers trans\r\n",trans_attr.points_number);
    }
	else if (trans_attr.points_number < 3 && type == 0)
	{
		printf("%d points too few to affine \r\n",trans_attr.points_number);
	}
    else 
    {
        for (int i = 0; i < trans_attr.points_number ; i++)
	    {
			dst_pt[i].x = trans_attr.dst_points[i].s32X;
			dst_pt[i].y = trans_attr.dst_points[i].s32Y;
			src_pt[i].x = trans_attr.src_points[i].s32X;
			src_pt[i].y = trans_attr.src_points[i].s32Y;
			
			if (img->enType == IVE_IMAGE_TYPE_YUV420SP)
			{
				dst_pt_uv[i].x = trans_attr.dst_points[i].s32X/2;
				dst_pt_uv[i].y = trans_attr.dst_points[i].s32Y/2;
				src_pt_uv[i].x = trans_attr.src_points[i].s32X/2;
				src_pt_uv[i].y = trans_attr.src_points[i].s32Y/2;
			}
	    }
    }
	timespec tp;
	int t =  clock_gettime(CLOCK_REALTIME, &tp);
	auto time0 = tp.tv_sec*1000+tp.tv_nsec/1000000;
	//cv::Mat transform_Y = cv::findHomography(points, points_transformed,CV_RANSAC, 2);
	cv::Mat transform_Y;
	if (type == 1)
		transform_Y = cv::getPerspectiveTransform(src_pt,dst_pt);
	else transform_Y = cv::getAffineTransform(src_pt,dst_pt);
	t =  clock_gettime(CLOCK_REALTIME, &tp);
	auto time00 = tp.tv_sec*1000+tp.tv_nsec/1000000;
	printf("Y Maritx calc time %d\r\n",time00 - time0);
	if  (img->enType == IVE_IMAGE_TYPE_YUV420SP)
	{
		cv::Mat Src_Mat(img->height,img->width,CV_8UC1, (unsigned char *)img->vir_addr[0]);
		cv::Mat UV_Mat(img->height/2,img->width/2,CV_8UC2, (unsigned char *)img->vir_addr[1]);

		t =  clock_gettime(CLOCK_REALTIME, &tp);
		auto time1 = tp.tv_sec*1000+tp.tv_nsec/1000000;
		cv::Mat transform_UV;
		if (type == 1)
			transform_UV = getPerspectiveTransform(src_pt_uv,dst_pt_uv);
		else transform_UV = getAffineTransform(src_pt_uv,dst_pt_uv);
		t =  clock_gettime(CLOCK_REALTIME, &tp);
		auto time2 = tp.tv_sec*1000+tp.tv_nsec/1000000;
		printf("UV Maritx calc time %d\r\n",time2 - time1);
		cv::Mat Y;
		cv::Mat UV;
		if (type == 1)
		{	
			cv::warpPerspective(UV_Mat,UV,transform_UV,cv::Size(UV_Mat.rows, UV_Mat.cols),cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
			cv::warpPerspective(Src_Mat,Y,transform_Y,cv::Size(Src_Mat.rows, Src_Mat.cols),cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
		}
		else 
		{
			cv::warpAffine(UV_Mat,UV,transform_UV,cv::Size(UV_Mat.rows, UV_Mat.cols),cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
			cv::warpAffine(Src_Mat,Y,transform_Y,cv::Size(Src_Mat.rows, Src_Mat.cols),cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
		}
		t =  clock_gettime(CLOCK_REALTIME, &tp);
		auto time3 = tp.tv_sec*1000+tp.tv_nsec/1000000;
		printf("Warp calc time %d\r\n",time3 - time2);
		ai_malloc_image_mmz_cached(img_res,Y.cols, Y.rows);
		(*img_res)->width  = Y.cols;
        (*img_res)->height = Y.rows;
		(*img_res)->channel = img->channel;
		memcpy((*img_res)->vir_addr[0],Y.data,Y.cols*Y.rows);
		memcpy((*img_res)->vir_addr[1],UV.data,Y.cols*Y.rows/2);
		//cv::imwrite("Y.jpg",Y);
		//cv::imwrite("SY.jpg",Src_Mat);
		//HI_MPI_SYS_MmzFlushCache((HI_U64)img->phy_addr[0],img->vir_addr[0],img->width*img->height*3/2);
		/*HI_S32 u32Stride = ALIGN_UP(Y.cols, 16);
        HI_S32 u32Size = u32Stride * ALIGN_UP(Y.rows, 16);
		//HI_MPI_SYS_MmzFlushCache((HI_U64)img->phy_addr[0], img->vir_addr[0], u32Size*3/2);
		printf("before mmap\r\n");
		HI_U8 * pUserPageAddr = (HI_U8 *)HI_MPI_SYS_MmapCache((HI_U64)(*img_res)->phy_addr[0], u32Size*3/2);
		if (HI_NULL == pUserPageAddr)
        {
            printf("IVE HI_MPI_SYS_Mmap fail!!! u32Size=%d\n",  u32Size);
        }
		else 
		{
			cv::Mat test(Y.rows*3/2, Y.cols, CV_8UC1,pUserPageAddr);
			cv::Mat rgb;
			cv::cvtColor(test, rgb, CV_YUV2BGR_NV21);
			cv::imwrite("ddd.jpg",rgb);
			HI_MPI_SYS_Munmap(pUserPageAddr, u32Size);
		}*/
	}
	return 0;
}





#ifdef __cplusplus
}
#endif //@__cplusplus
