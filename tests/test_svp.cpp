#include <stdio.h>
#include <string.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "KitchenCls.h"
#include "kitchen_interface.h"
#include "kitchen_utils.h"
#ifdef __cplusplus
extern "C" {
#endif

/*#include "ai_comm_sys.h"
#include "ai_comm_vdec.h"
#include "ai_comm_vpss.h"*/
//#include "sample_nnie_main.h"
#include <pthread.h>
#include "ai_global.h"
#include "ai_npu_sdk.h"
#include "aibox.h"

using namespace std;

const int MAX_W = 1920;
const int MAX_H = 1080;
const int my_test_chn = 4;
pthread_t  the_thread;
pthread_t  video_thread;
bool isend = false;
int step = 0;
bool do_kitchen_detect = false;
bool test_vpss=true;

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
    int nframe  =0;
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

        // nframe ++;
        // if (nframe % (5*25*10) != 0){
        //     continue;
        // }
        // cout<<"nframe:"<<nframe<<endl;


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

void * video_data_test(void * arg)
{
    std::cout<<"video_data_test start"<<std::endl;
    AI_IMAGE_S * img = NULL;
    struct timeval tv1;
	struct timeval tv2;
	long t1, t2, time, totaltime = 0, nim = 0;
    // int cam_filter[10]={1,0,1, 1,1,1, 1,1,1, 1};
    // kitchen_cam_init(my_test_chn, cam_filter);
    FIRE_TRASH_ST st;
    RECT_ST roi0, roi1;
    roi0.x = 1665./1920;
    roi0.y = 423./1080;
    //1890, 645
    roi0.w = (1890.-1665)/1920;
    roi0.h = (645.-423)/1080;
    roi0.type = 1;
    st.nrect = 1;
    st.rects[0] = roi0;
    //set_roi(my_test_chn, st);
    usleep(1000*1000);
    //for (int i = 0; i < 1 ; i ++)

    while(true)
    {
        std::cout<<"start receive"<<std::endl;
        int nret = ai_video_decode_receive_frame(my_test_chn, &img);
        
        std::cout<<"ai_video_decode_receive_frame ret:"<<nret<<std::endl;
        if (do_kitchen_detect && (nret==0)){
            //kc.run_yvu420sp(my_test_chn, &img);
            ST_PERSON_BOX_ATTR arr[50];
            int arrlen = 50;
            std::cout<<"call kitchen_detect_vpss arr"<<arr<<std::endl;
            nim += 1;
            ST_IMAGE_STATE im_state;
          	gettimeofday(&tv1, NULL);
            if (test_vpss){
                kitchen_state_vpss(my_test_chn, &img, &im_state);
                std::cout<<"im_state:"<< im_state.cover<<std::endl;
                //kitchen_detect_vpss(my_test_chn, img, arr, &arrlen);
            }else{
                kitchen_detect_opencv(my_test_chn, img, arr, &arrlen);
                for (int i =0; i< arrlen; i++){
                std::cout<<"personboxattr clsid:"<<std::endl;
                std::cout<< arr[i].cls_id <<" score:" << arr[i].score << " hat:" << arr[i].hat \
                        <<" kouzhao:"<<arr[i].mask<<" cigarette:"<<arr[i].cigarette<<" shouji:"<<arr[i].shouji \
                        <<" nude:"<<arr[i].nude<< " uniform:"<<arr[i].uniform \
                        <<" can_open:"<<arr[i].can_open<<" mouse:"<<arr[i].mouse<<std::endl;
            }
            }            
            gettimeofday(&tv2, NULL);
        	t1 = tv2.tv_sec - tv1.tv_sec;
	        t2 = tv2.tv_usec - tv1.tv_usec;
	        time = (long)(t1 * 1000 + t2 / 1000);
            totaltime += time;
            printf("this time : %dms\n", time);
            time = totaltime / nim;
	        printf("average time : %dms\n", time);
            //usleep(1000*1000);
            //kitchen_destroy_vpss(my_test_chn);
            //usleep(1000*1000);
            //std::cout<<"recog nperson:"<<arrlen<<std::endl;
            //hat, mask, cigarette, shouji, nude, uniform
            
        }
        if (nret != 0){
            break;
        }
        ai_video_decode_release_frame(img);
    }

    isend = true;
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



    isend = true;
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
    while (!isend)
    {
        usleep(1000);
    }
    ai_video_decode_exit(my_test_chn);
    ai_exit();
  
}


void video_file_func()
{
    int nret = 0;
    printf("load video_func\n");
    ai_init();
    
    int cam_filter[10]={1,0,1, 1,1,1, 1,1,1, 1};
    kitchen_cam_init(my_test_chn, cam_filter);

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
    cv::Mat frame;
    VDEC_THREAD_PARAM_S stVdecSend;
    std::string dname = "/mnt/mtd/nfs/zhangjianwei/lib/data/videos/";
    //std::string vfname("Ch5_20201028072815.h264");
    std::string vfname("Ch7_20201028082017.h264");
    //std::string vfname("Ch8_20201028060059.h264");
    //std::string vfname("Ch9_20201028062516.h264");
    //std::string vfname("Ch10_20201028042606.h264");
    //std::string vfname("Ch11_20201028071610.h264");
    //std::string vfname("Ch12_20201028074252.h264");
    
    snprintf(stVdecSend.cFileName, sizeof(stVdecSend.cFileName), vfname.c_str());
    snprintf(stVdecSend.cFilePath, sizeof(stVdecSend.cFilePath), "%s", dname.c_str());

    stVdecSend.enType          = PT_H264;
    stVdecSend.s32StreamMode   = VIDEO_MODE_FRAME;
    stVdecSend.s32ChnId        = my_test_chn;
    stVdecSend.s32IntervalTime = 100;
    stVdecSend.u64PtsInit      = 0;
    stVdecSend.u64PtsIncrease  = 0;
    stVdecSend.eThreadCtrl     = THREAD_CTRL_START;
    stVdecSend.bCircleSend     = HI_FALSE;
    stVdecSend.s32MilliSec     = 0;
    stVdecSend.s32MinBufSize   = (1920 * 1080 * 3)>>1;
    SAMPLE_COMM_VDEC_SendStream((void *)&stVdecSend);
    //SAMPLE_COMM_VDEC_StartSendStream(s32VdecChnNum, &stVdecSend[0], &VdecThread[0]);
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


    std::string dname = "/mnt/mtd/nfs/zhangjianwei/lib/data/test_images/yolov3/jpg/";
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
        break;
    }
    usleep(1000*1000*60);
    std::cout<<"send file finish, ai_video_decode_exit"<<std::endl;
    ai_video_decode_exit(my_test_chn);
    ai_exit();
}

void test_error_image(){
    std::string dname = "/mnt/mtd/nfs/zhangjianwei/green_images/";
    DIR *d;
    struct dirent *dir;
    d = opendir(dname.c_str());
    if (!d){
        printf("dir read error");
        return;
    }
    std::ofstream logf("log.txt");
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
        cv::Mat im = cv::imread(imPath);
        cv::cvtColor(im, im, cv::COLOR_BGR2YUV_I420);

        bool errim = detect_green_img(im.data, im.cols, im.rows, true, logf);
        if (errim){
            std::cout<<"err image"<<std::endl;
        }
    }
}

void test_kitchen_interface()
{
    do_kitchen_detect = true;

    if (test_vpss){
        //vpss yvu420sp
        // kitchen_init("/mnt/mtd/nfs/zhangjianwei/lib/data/models/yolov3_inst2cls_yvu420sp_960_544_inst.wk" \
        //             , "/mnt/mtd/nfs/zhangjianwei/lib/data/models/attr_kitchen6_yvu420sp_inst.wk","./data/conf/conf.txt", "./data/log/");
        kitchen_init_with_dir("/mnt/mtd/nfs/zhangjianwei/lib/data/");
    }else{
        //opencv
        kitchen_init("/mnt/mtd/nfs/zhangjianwei/lib/data/models/yolov3_inst2cls_960_544_opencvbgr_inst.wk" \
                    , "/mnt/mtd/nfs/zhangjianwei/lib/data/models/attr_kitchen6_opencvbgr_inst.wk", "", "./data/log/");
    }
    
    //cam_filter: {{0, "hat"}, {1, "mask"}, {2, "cigarette"},  {3, "shouji"}, {4, "nude"}, {5, "uniform"},
    //             {6,"fire"}, {7,"ashcan_open"}, {8,"mouse"}, {9,"cover"}}
    video_file_func();
    //img_dir_func();
}
struct abc{
    int a;
    int b;
};
void test(struct abc *a){
    std::cout<<a[0].a<<a[0].b<<a[1].a<<a[1].b<<std::endl;
    // a[0].a=7;
    // a[0].b=8;
    // a[1].a=0;
    // a[1].b=1;
    struct abc b[2]={{2,3},{4,5}};
    a[0]=b[0];
    a[1]=b[1];
}
int main(int argc, char *argv[])
{
    //img_dir_func();
    test_kitchen_interface();
    //test_error_image();
    return HI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
