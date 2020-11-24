#include <stdio.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#ifdef __cplusplus
extern "C" {
#endif

/*#include "ai_comm_sys.h"
#include "ai_comm_vdec.h"
#include "ai_comm_vpss.h"*/
//#include "sample_nnie_main.h"
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <dirent.h>


#include "ai_global.h"
#include "ai_npu_sdk.h"
#include "sample_comm.h"
#include "aibox.h"


//using namespace std;
//using namespace cv;

#define THREAD_NUM 1
const int MAX_W = 1920;
const int MAX_H = 1080;
const int my_test_chn = 5;
const int my_test_chn_arr[16] = {0, 1,2,3,4,5,6,7, 8,9,10,11,12,13,14,15};

pthread_t  the_thread;
pthread_t  video_thread;
pthread_t  video_threads[16];
pthread_t  video_threads_send[16];
bool end = false;
int step = 0;
bool test_detect=true;

HI_VOID * SAMPLE_COMM_VDEC_SendStream(HI_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    HI_BOOL bEndOfStream = HI_FALSE;
    HI_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm=NULL;
    HI_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    HI_BOOL bFindStart, bFindEnd;
    HI_U64 u64PTS = 0;
    HI_U32 u32Len, u32Start;
    HI_S32 s32Ret,  i;
    HI_CHAR cStreamFile[256];
    HI_U32 iim = -1;
    HI_U32 perim = 10;

    prctl(PR_SET_NAME, "VideoSendStream", 0,0,0);

    snprintf(cStreamFile, sizeof(cStreamFile), "%s/%s", pstVdecThreadParam->cFilePath,pstVdecThreadParam->cFileName);
    if(cStreamFile != 0)
    {
        fpStrm = fopen(cStreamFile, "rb");
        if(fpStrm == NULL)
        {
            SAMPLE_PRT("chn %d can't open file %s in send stream thread!\n", pstVdecThreadParam->s32ChnId, cStreamFile);
            return (HI_VOID *)(HI_FAILURE);
        }
    }
    printf("\n \033[0;36m chn %d, stream file:%s, userbufsize: %d \033[0;39m\n", pstVdecThreadParam->s32ChnId,
        pstVdecThreadParam->cFileName, pstVdecThreadParam->s32MinBufSize);

    pu8Buf = (HI_U8 *)malloc(pstVdecThreadParam->s32MinBufSize);
    if(pu8Buf == NULL)
    {
        SAMPLE_PRT("chn %d can't alloc %d in send stream thread!\n", pstVdecThreadParam->s32ChnId, pstVdecThreadParam->s32MinBufSize);
        fclose(fpStrm);
        return (HI_VOID *)(HI_FAILURE);
    }
    fflush(stdout);

    u64PTS = pstVdecThreadParam->u64PtsInit;
    while (1)
    {

        bEndOfStream = HI_FALSE;
        bFindStart   = HI_FALSE;
        bFindEnd     = HI_FALSE;
        u32Start     = 0;
        fseek(fpStrm, s32UsedBytes, SEEK_SET);
        s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
        if (s32ReadLen == 0)
        {
            if (pstVdecThreadParam->bCircleSend == HI_TRUE)
            {
                memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
                stStream.bEndOfStream = HI_TRUE;
                HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

                s32UsedBytes = 0;
                fseek(fpStrm, 0, SEEK_SET);
                s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
            }
            else
            {
                break;
            }
        }

        if (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME && pstVdecThreadParam->enType == PT_H264)
        {
            //std::cout<<"121"<<std::endl;
            for (i=0; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (  pu8Buf[i    ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                       (
                           ((tmp == 0x5 || tmp == 0x1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                           (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)
                        )
                   )
                {
                    bFindStart = HI_TRUE;
                    i += 8;
                    break;
                }
            }
            //std::cout<<"137"<<std::endl;
            for (; i<s32ReadLen-8; i++)
            {
                int tmp = pu8Buf[i+3] & 0x1F;
                if (  pu8Buf[i    ] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1 &&
                            (
                                  tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 ||
                                  ((tmp == 5 || tmp == 1) && ((pu8Buf[i+4]&0x80) == 0x80)) ||
                                  (tmp == 20 && (pu8Buf[i+7]&0x80) == 0x80)
                              )
                   )
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }
            //std::cout<<"152"<<std::endl;
            if(i>0)s32ReadLen = i;
            if (bFindStart == HI_FALSE)
            {
                SAMPLE_PRT("chn %d can not find H264 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                    pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+8;
            }
            //std::cout<<"164"<<std::endl;

        }
        else if (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME
            && pstVdecThreadParam->enType == PT_H265)
        {
            HI_BOOL  bNewPic = HI_FALSE;
            for (i=0; i<s32ReadLen-6; i++)
            {
                HI_U32 tmp = (pu8Buf[i+3]&0x7E)>>1;
                bNewPic = (HI_BOOL)( pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
                            && (tmp >= 0 && tmp <= 21) && ((pu8Buf[i+5]&0x80) == 0x80) );

                if (bNewPic)
                {
                    bFindStart = HI_TRUE;
                    i += 6;
                    break;
                }
            }

            for (; i<s32ReadLen-6; i++)
            {
                HI_U32 tmp = (pu8Buf[i+3]&0x7E)>>1;
                bNewPic = (HI_BOOL)(pu8Buf[i+0] == 0 && pu8Buf[i+1] == 0 && pu8Buf[i+2] == 1
                            &&( tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 || ((tmp >= 0 && tmp <= 21) && (pu8Buf[i+5]&0x80) == 0x80) )
                             );

                if (bNewPic)
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }
            if(i>0)s32ReadLen = i;

            if (bFindStart == HI_FALSE)
            {
                SAMPLE_PRT("chn %d can not find H265 start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                    pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
            if (bFindEnd == HI_FALSE)
            {
                s32ReadLen = i+6;
            }

        }
        else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG)
        {
            for (i=0; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    u32Start = i;
                    bFindStart = HI_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i<s32ReadLen-3; i++)
            {
                if ((pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0)
                {
                     u32Len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                     i += 1 + u32Len;
                }
                else
                {
                    break;
                }
            }

            for (; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD9)
                {
                    bFindEnd = HI_TRUE;
                    break;
                }
            }
            s32ReadLen = i+2;

            if (bFindStart == HI_FALSE)
            {
                SAMPLE_PRT("chn %d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                    pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
        }
        else
        {
            if((s32ReadLen != 0) && (s32ReadLen < pstVdecThreadParam->s32MinBufSize))
            {
                bEndOfStream = HI_TRUE;
            }
        }

        stStream.u64PTS       = u64PTS;
        stStream.pu8Addr      = pu8Buf + u32Start;
        stStream.u32Len       = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? HI_TRUE: HI_FALSE;
        stStream.bEndOfStream = bEndOfStream;
        stStream.bDisplay     = (HI_BOOL)1;
        //std::cout<<"267"<<std::endl;
SendAgain:
        // iim += 1;
        // if (iim%perim != 0){
        //     bEndOfStream = HI_FALSE;
        //     s32UsedBytes = s32UsedBytes +s32ReadLen + u32Start;
        //     u64PTS += pstVdecThreadParam->u64PtsIncrease;
        //     continue;
        // }
        s32Ret=HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if( (HI_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl) )
        {
            usleep(pstVdecThreadParam->s32IntervalTime);
            goto SendAgain;
        }
        else
        {
            bEndOfStream = HI_FALSE;
            s32UsedBytes = s32UsedBytes +s32ReadLen + u32Start;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
        }
        usleep(pstVdecThreadParam->s32IntervalTime);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = HI_TRUE;
    HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    printf("\033[0;35m chn %d send steam thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != HI_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (HI_VOID *)HI_SUCCESS;
}



//KitchenCls kc;

void * video_data_test(void * arg)
{
    std::cout<<"video_data_test start"<<std::endl;
    AI_IMAGE_S * img = NULL;
    struct timeval tv1;
	struct timeval tv2;
	long t1, t2, time, totaltime = 0, nim = 0;
    int ithread = *((int *)arg);
    usleep(1000*1000);
    //for (int i = 0; i < 1 ; i ++)
    ST_BOX boxes[MAX_BOX_NUM];
    memset(boxes, 0, sizeof(ST_BOX)*MAX_BOX_NUM);
    int nbox = 0;
    while(true)
    {
        std::cout<<ithread<<" start receive"<<std::endl;
        int nret = ai_video_decode_receive_frame(my_test_chn_arr[ithread], &img);
        
        std::cout<<"ai_video_decode_receive_frame ret:"<<nret<<std::endl;
        if (test_detect && (nret==0)){
            //kc.run_yvu420sp(my_test_chn, &img);
            int arrlen = 0;
            nim += 1;
          	gettimeofday(&tv1, NULL);
            nbox=0;
            std::cout<<"call ai_detect"<<std::endl;
            ai_detect("yolov3", img, boxes, &nbox);
        	t1 = tv2.tv_sec - tv1.tv_sec;
	        t2 = tv2.tv_usec - tv1.tv_usec;
	        time = (long)(t1 * 1000 + t2 / 1000);
            totaltime += time;
            printf("this time:%d ms", time);
            time = totaltime / nim;
	        printf("average time : %dms\n", time);
            //usleep(1000*1000);
            //kitchen_destroy_vpss(my_test_chn);
            //usleep(1000*1000);
            // std::cout<<"recog nperson:"<<arrlen<<std::endl;
            //hat, mask, cigarette, shouji, nude, uniform
            // for (int i =0; i< arrlen; i++){
            //     std::cout<<"personboxattr clsid:"<<std::endl;
            //     std::cout<< arr[i].cls_id <<" score:" << arr[i].score << " hat:" << arr[i].hat \
            //         <<" kouzhao:"<<arr[i].mask<<" cigarette:"<<arr[i].cigarette<<" shouji:"<<arr[i].shouji \
            //         <<" nude:"<<arr[i].nude<< " uniform:"<<arr[i].uniform<<std::endl;
            // }
            //delete_person_box_attr(arr, arrlen);
            
        }
        if (nret != 0){
            break;
        }
        ai_video_decode_release_frame(img);
    }

    end = true;
    return arg;
}
void * get_data_test(void * arg)
{
    PERSTRANS_PARA attr;
    attr.points_number = 4;
    AI_POINT_F src_pt[4];
    AI_POINT_F dst_pt[4];
    src_pt[0].s32X = 10, src_pt[0].s32Y = 10;
    src_pt[1].s32X = 690, src_pt[1].s32Y = 10;
    src_pt[2].s32X = 690, src_pt[2].s32Y = 690;
    src_pt[3].s32X = 10, src_pt[3].s32Y = 690;
   
    dst_pt[0].s32X = 490, dst_pt[0].s32Y = 210;
    dst_pt[1].s32X = 490, dst_pt[1].s32Y = 490;
    dst_pt[2].s32X = 210, dst_pt[2].s32Y = 490;
    dst_pt[3].s32X = 210, dst_pt[3].s32Y = 210;
    
    attr.src_points = src_pt;
    attr.dst_points = dst_pt;
    AI_IMAGE_S * img = NULL;
    AI_IMAGE_S * img_trans = NULL;
    usleep(1000*1000);
    ai_video_decode_receive_frame(my_test_chn,&img);
    printf("prepare to perstrans\r\n");
    img->enType = IVE_IMAGE_TYPE_YUV420SP;
    ai_frame_ive_perstrans(img,&img_trans,attr);
    printf("prepare to save image 1\r\n");
    ai_save_image("save1.jpg", img);
    printf("prepare to save image trans 1,%d,%d\r\n",img_trans->width,img_trans->height);
    ai_save_image("new1.jpg", img_trans);
    printf("prepare to release 1\r\n");
    ai_video_decode_release_frame(img);
    printf("prepare to release trans 1\r\n");
    ai_video_decode_release_frame(img_trans);
    step = 1;
    ai_video_decode_receive_frame(my_test_chn,&img);
    printf("prepare to save image 2\r\n");
    ai_save_image("save2.jpg", img);
    ai_video_decode_release_frame(img);
    usleep(1000*1000*5);
    step = 2;
    ai_video_decode_receive_frame(my_test_chn,&img);
    printf("prepare to save image 3\r\n");
    //ai_save_image("save3.jpg", img);
    dst_pt[0].s32X = 10, dst_pt[0].s32Y = 10;
    dst_pt[1].s32X = 1270, dst_pt[1].s32Y = 10;
    dst_pt[2].s32X = 1270, dst_pt[2].s32Y = 710;
    dst_pt[3].s32X = 10, dst_pt[3].s32Y = 710;
   
    src_pt[0].s32X = 900, src_pt[0].s32Y = 10;
    src_pt[1].s32X = 1700, src_pt[1].s32Y = 360;
    src_pt[2].s32X = 700, src_pt[2].s32Y = 1100;
    src_pt[3].s32X = -310, src_pt[3].s32Y = 710;
    img->enType = IVE_IMAGE_TYPE_YUV420SP;
    usleep(1000*1000*5);
    ai_frame_ive_perstrans(img,&img_trans,attr);
    ai_save_image("save3.jpg", img);
    ai_save_image("new3.jpg", img_trans);
    ai_video_decode_release_frame(img);
    ai_video_decode_release_frame(img_trans);



    end = true;
    return arg;
   
}
void test_func()
{
    
    ai_init();

    ai_video_decode_init(my_test_chn);
    printf("init done,version 727\r\n");
    
    FILE* pFile = fopen("/mnt/mtd/nfs/ImageTest/22.jpg", "rb");
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    char* pData = new char[lSize];
    fread(pData, sizeof(char), lSize, pFile);
    fclose(pFile);

    printf("load file\r\n");
    pthread_create(&the_thread, 0, get_data_test, NULL);
    ai_video_start_vo(my_test_chn);
    SIZE_S _rect;
    RECT_S _crop;
    _rect.u32Height = 1280; _rect.u32Width= 2520;
    _crop.s32X = 119; _crop.s32Y = 50;
    _crop.u32Width = 800; _crop.u32Height = 600;
    
    ai_video_decode_send_frame(my_test_chn,pData,(int)lSize, 0);
    while (step < 1)
    {
        usleep(10);
    }
    ai_frame_vdec_vpss_connect(my_test_chn,HI_TRUE);
    ai_frame_vpss_setcrop(my_test_chn,_crop);
    ai_frame_vpss_setframesize(my_test_chn,_rect);
    ai_video_decode_send_frame(my_test_chn,pData,(int)lSize, 0);
    while (step < 2)
    {
        usleep(10);
    }
    _rect.u32Height = 1480;
    _rect.u32Width = 2220;
    _crop.s32X = 33; _crop.s32Y = 50;
    _crop.u32Width = 0; _crop.u32Height = 300;
    ai_frame_vpss_setcrop(my_test_chn,_crop);
    ai_frame_vpss_setframesize(my_test_chn,_rect);
    ai_video_decode_send_frame(my_test_chn,pData,(int)lSize, 0);
    printf("ai_video_decode_send_frame\r\n");
    delete pData;
    while (!end)
    {
        usleep(1000);
    }
    ai_video_decode_exit(my_test_chn);
    ai_exit();
  
}


void video_file_func()
{
    cv::Mat frame;
    VDEC_THREAD_PARAM_S stVdecSend[16];
    std::string dname = "/mnt/mtd/nfs/zhangjianwei/lib/data/videos/";
    std::string vfnames[16]={"Ch5_20201028072815.h264", "Ch7_20201028082017.h264", "a.h264", "cam_mouse.h264" \
                            , "a.h264", "cam_mouse.h264", "Ch5_20201028072815.h264", "Ch7_20201028082017.h264" \
                            , "a.h264", "cam_mouse.h264", "Ch5_20201028072815.h264", "Ch7_20201028082017.h264" \
                            , "a.h264", "cam_mouse.h264", "Ch5_20201028072815.h264", "Ch7_20201028082017.h264"};
                            
                            // , "cover11.h264", "cover10.h264", "cover0.h264", "cover1.h264" \
                            // , "cover2.h264", "cover3.h264", "cover4.h264", "cover5.h264" \
                            // , "cover6.h264", "cover7.h264", "cover8.h264", "cover9.h264"};

    int nret = 0;
    printf("load video_func\n");
    ai_init();
    int args[16];
    for(int i =0; i<THREAD_NUM; i++){
        args[i] = i;
        ai_video_decode_init(my_test_chn_arr[i]);
        ai_frame_vdec_vpss_connect(my_test_chn_arr[i],HI_TRUE);
        pthread_create(&video_threads[i], 0, video_data_test, (void *)&args[i]);
        SIZE_S _rect;
        RECT_S _crop;
        _rect.u32Height = 1080; _rect.u32Width= 1920;
        _crop.s32X = 0; _crop.s32Y = 0;
        _crop.u32Width = 0; _crop.u32Height = 0;
        ai_frame_vpss_setframesize(my_test_chn_arr[i],_rect);

        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), vfnames[i].c_str());
        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", dname.c_str());

        stVdecSend[i].enType          = PT_H264;
        stVdecSend[i].s32StreamMode   = VIDEO_MODE_FRAME;
        stVdecSend[i].s32ChnId        = my_test_chn_arr[i];
        stVdecSend[i].s32IntervalTime = 10;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = HI_TRUE;
        stVdecSend[i].s32MilliSec     = 0;
        stVdecSend[i].s32MinBufSize   = (1920 * 1080 * 3)>>1;

        pthread_create(&video_threads_send[i], 0, SAMPLE_COMM_VDEC_SendStream, (void *)&stVdecSend[i]);
    }
    for(int i =0; i<THREAD_NUM;i++){
        pthread_join(video_threads_send[i], NULL );
    }
    std::cout<<"video file read finish"<<std::endl;
}


void img_dir_func()
{
    int nret = 0;
    printf("load video_func\n");
    ai_init();
    ai_video_decode_init(my_test_chn);
    printf("load file\n");
    // ai_video_start_vo(my_test_chn);
    ai_frame_vdec_vpss_connect(my_test_chn,HI_TRUE);
    pthread_create(&video_thread, 0, video_data_test, NULL);
    SIZE_S _rect;
    RECT_S _crop;
    _rect.u32Height = 1080; _rect.u32Width= 1920;
    _crop.s32X = 0; _crop.s32Y = 0;
    _crop.u32Width = 0; _crop.u32Height = 0;
    ai_frame_vpss_setframesize(my_test_chn,_rect);

    std::string dname = "/mnt/mtd/nfs/zhangjianwei/aibox/data/test_images/yolov3/jpg";
    DIR *d;
    struct dirent *dir;
    while(true){
        d = opendir(dname.c_str());
        if (!d){
            printf("dir read error");
            return;
        }

        while ((dir = readdir(d)) != NULL)
        {
            printf("parse %s\n", dir->d_name);
            std::string imname(dir->d_name);
            int fnlen = imname.size();
            if (fnlen<4){continue;}
            int jpgExtPos = fnlen - 4;
            if (!(imname.substr(jpgExtPos) == ".jpg")) continue;
            //std::cout<<"will send:"<<imname<<std::endl;
            std::string imPath = dname + imname;
            FILE* pFile = fopen(imPath.c_str(), "rb");
            fseek(pFile, 0, SEEK_END);
            long lSize = ftell(pFile);
            rewind(pFile);
            char* pData = new char[lSize];
            fread(pData, sizeof(char), lSize, pFile);
            fclose(pFile);
            nret = ai_video_decode_send_frame(my_test_chn,pData,(int)lSize, 0);
            std::cout<<"ai_video_decode_send_frame ret:"<<nret<<std::endl;
            
            delete [] pData;
            //std::cout<<"image sent:"<<imname<<std::endl;
            usleep(1000*1000*1);
        }
        closedir(d);
        
    }
    usleep(1000*1000*60);
    std::cout<<"send file finish, ai_video_decode_exit"<<std::endl;
    ai_video_decode_exit(my_test_chn);
    ai_exit();
}
void test_kitchen_interface()
{
    if (test_detect){
        //vpss yvu420sp
        // kitchen_init("/mnt/mtd/nfs/zhangjianwei/lib/data/models/yolov3_inst2cls_yvu420sp_960_544_inst.wk" \
        //             , "/mnt/mtd/nfs/zhangjianwei/lib/data/models/attr_kitchen6_yvu420sp_inst.wk");
        //kitchen_init_with_dir("/mnt/mtd/nfs/zhangjianwei/lib/data/");

        // AI_DETECT_ARGS_ST argst{.image_width=1920, .image_width=1080, .class_num=80, .do_nms=0, .nms_thresh=0.5, \
        //                         .conf_thresh=0.1, .data_dir="/mnt/mtd/nfs/zhangjianwei/aibox/data", .model_name="yolov3"};
        AI_DETECT_ARGS_ST argst{416, 416, 80, 1, 0.5, 0.1, "/mnt/mtd/nfs/zhangjianwei/aibox/data", "yolov3"};
        ai_model_init(argst);
    }else{
    }
    //img_dir_func();
    video_file_func();
    
}
// void test_nnie_kitchen()
// {
//     //SAMPLE_SVP_NNIE_Cnn();
//     SAMPLE_SVP_NNIE_Yolov3();

// }
int main(int argc, char *argv[])
{
    // int m,n;
    // for (m = 0; m < 256; m++)
    //     for (n = 0; n < 16 * 8 * 10; n++)
    //         HI_MPI_VB_ReleaseBlock(m * 65536 + n);

    // for (n = 0; n < 24; n++)
    //     HI_MPI_VB_ExitModCommPool((VB_UID_E)n);

    //kc.init("/mnt/mtd/nfs/zhangjianwei/lib/data/models/yolov3_inst_kitchen_inst.wk", "/mnt/mtd/nfs/zhangjianwei/lib/data/models/attr_kitchen7_inst.wk");
    //test_func();
    //img_dir_func();
    test_kitchen_interface();
    printf("init done,version 904\r\n");
  

    return HI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
