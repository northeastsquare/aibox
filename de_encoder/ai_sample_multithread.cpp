#include <stdio.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "KitchenCls.h"
#include "kitchen_interface.h"
#ifdef __cplusplus
extern "C" {
#endif


/*#include "ai_comm_sys.h"
#include "ai_comm_vdec.h"
#include "ai_comm_vpss.h"*/
//#include "sample_nnie_main.h"

#include "aibox.h"
#include <pthread.h>
const int MAX_W = 1920;
const int MAX_H = 1080;
const int my_test_chn = 34;
const int my_test_chn_arr[2] = {34, 38};
//const int my_test_chn1 = 38;
pthread_t  the_thread;
pthread_t  video_thread0;
pthread_t  video_thread1;
bool end = false;
int step = 0;
bool do_kitchen_detect = false;
//KitchenCls kc;

void * video_data_test(void * arg)
{
    
    AI_IMAGE_S * img = NULL;
    struct timeval tv1;
	struct timeval tv2;
	long t1, t2, time, totaltime = 0, nim = 0;
    int ithread = *((int *)arg);
    usleep(1000*1000);
    

    //for (int i = 0; i < 1 ; i ++)
    while(true)
    {
        std::cout<<ithread<<"thread start receive"<<std::endl;
        int nret = ai_video_decode_receive_frame(my_test_chn_arr[ithread], &img);
        
        std::cout<<"ai_video_decode_receive_frame ret:"<<nret<<std::endl;
        if (do_kitchen_detect && (nret==0)){
            //kc.run_yvu420sp(my_test_chn, &img);
            ST_PERSON_BOX_ATTR *arr = NULL;
            int arrlen = 0;
            //std::cout<<"call kitchen_detect_vpss"<<std::endl;
            nim += 1;
          	gettimeofday(&tv1, NULL);
            kitchen_detect_vpss(my_test_chn_arr[ithread], &img, &arr, &arrlen);
            //kitchen_detect_opencv(my_test_chn, &img, &arr, &arrlen);
            gettimeofday(&tv2, NULL);
        	t1 = tv2.tv_sec - tv1.tv_sec;
	        t2 = tv2.tv_usec - tv1.tv_usec;
	        time = (long)(t1 * 1000 + t2 / 1000);
            totaltime += time;
            time = totaltime / nim;
	        //printf("average time : %dms\n", time);

            std::cout<<"recog nperson:"<<arrlen<<std::endl;
            for (int i =0; i< arrlen; i++){
                std::cout<<"personboxattr clsid:"<<std::endl;
                std::cout<< arr[i].cls_id <<" score:" << arr[i].score << " hat:" << arr[i].hat <<std::endl;
            }
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
    // printf("load video_func\n");
    // ai_init();
    // ai_video_decode_init(my_test_chn0);
    // ai_video_decode_init(my_test_chn1);
    // printf("load file\n");
    // pthread_create(&video_thread0, 0, video_data_test, NULL);
    // pthread_create(&video_thread1, 0, video_data_test, NULL);
    // // ai_video_start_vo(my_test_chn);
    // ai_frame_vdec_vpss_connect(my_test_chn0,HI_TRUE);
    // ai_frame_vdec_vpss_connect(my_test_chn1,HI_TRUE);
    // SIZE_S _rect;
    // RECT_S _crop;
    // _rect.u32Height = 1080; _rect.u32Width= 1920;
    // _crop.s32X = 0; _crop.s32Y = 0;
    // _crop.u32Width = 0; _crop.u32Height = 0;
    // ai_frame_vpss_setframesize(my_test_chn0,_rect);
    // ai_frame_vpss_setframesize(my_test_chn1,_rect);

    // std::string dname = "/mnt/mtd/nfs/zhangjianwei/lib/data/test_images/yolov3/jpg/";
    // DIR *d;
    // struct dirent *dir;
    
    // while(true){
    //     d = opendir(dname.c_str());
    //     if (!d){
    //         printf("dir read error");
    //         return;
    //     }
    //     int iim = -1;
    //     while ((dir = readdir(d)) != NULL)
    //     {
    //         iim += 1;
    //         printf("parse %s\n", dir->d_name);
    //         std::string imname(dir->d_name);
    //         int fnlen = imname.size();
    //         if (fnlen<4){continue;}
    //         int jpgExtPos = fnlen - 4;
    //         if (!(imname.substr(jpgExtPos) == ".jpg")) continue;
    //         std::cout<<"will send:"<<imname<<std::endl;
    //         std::string imPath = dname + imname;
    //         FILE* pFile = fopen(imPath.c_str(), "rb");
    //         fseek(pFile, 0, SEEK_END);
    //         long lSize = ftell(pFile);
    //         rewind(pFile);
    //         char* pData = new char[lSize];
    //         fread(pData, sizeof(char), lSize, pFile);
    //         fclose(pFile);
    //         if (iim % 2 == 0){
    //             ai_video_decode_send_frame(my_test_chn0,pData,(int)lSize, 0);
    //         }
    //         else{
    //             ai_video_decode_send_frame(my_test_chn1,pData,(int)lSize, 0);
    //         }
    //         delete [] pData;
    //         std::cout<<"image sent:"<<imname<<std::endl;
    //         usleep(1000*1000*1);
    //     }
    // }
    // usleep(1000*1000*60);
    // std::cout<<"send file finish, ai_video_decode_exit"<<std::endl;
    // ai_video_decode_exit(my_test_chn0);
    // ai_video_decode_exit(my_test_chn0);
    // ai_exit();
}
void img_dir_func()
{
    int nret = 0;
    printf("load video_func\n");
    ai_init();
    ai_video_decode_init(my_test_chn_arr[0]);
    ai_video_decode_init(my_test_chn_arr[1]);
    printf("load file\n");
    // ai_video_start_vo(my_test_chn);
    ai_frame_vdec_vpss_connect(my_test_chn_arr[0],HI_TRUE);
    ai_frame_vdec_vpss_connect(my_test_chn_arr[1],HI_TRUE);
    int arg = 0;
    pthread_create(&video_thread0, 0, video_data_test, (void *)&arg);
    int arg1 = 1;
    pthread_create(&video_thread1, 0, video_data_test, (void *)&arg1);
    SIZE_S _rect;
    RECT_S _crop;
    _rect.u32Height = 1080; _rect.u32Width= 1920;
    _crop.s32X = 0; _crop.s32Y = 0;
    _crop.u32Width = 0; _crop.u32Height = 0;
    ai_frame_vpss_setframesize(my_test_chn_arr[0],_rect);
    ai_frame_vpss_setframesize(my_test_chn_arr[1],_rect);


    std::string dname = "/mnt/mtd/nfs/zhangjianwei/lib/data/test_images/yolov3/jpg/";
    DIR *d;
    struct dirent *dir;
    while(true){
        d = opendir(dname.c_str());
        if (!d){
            printf("dir read error");
            return;
        }
        int iim = -1;
        while ((dir = readdir(d)) != NULL)
        {
            iim += 1;
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
            ai_video_decode_send_frame(my_test_chn_arr[iim % 2],pData,(int)lSize, 0);

            std::cout<<"ai_video_decode_send_frame ret:"<<nret<<std::endl;
            
            delete [] pData;
            //std::cout<<"image sent:"<<imname<<std::endl;
            usleep(1000*1000*1);
        }
        closedir(d);

    }
    usleep(1000*1000*60);
    std::cout<<"send file finish, ai_video_decode_exit"<<std::endl;
    ai_video_decode_exit(my_test_chn_arr[0]);
    ai_video_decode_exit(my_test_chn_arr[1]);
    ai_exit();
}
void test_kitchen_interface()
{
    do_kitchen_detect = true;
    //vpss yvu420sp
    kitchen_init("/mnt/mtd/nfs/zhangjianwei/lib/data/models/yolov3_inst2cls_yvu420sp_960_544_inst.wk" \
                , "/mnt/mtd/nfs/zhangjianwei/lib/data/models/attr_kitchen7_yvu420sp_inst.wk");
    //opencv
    // kitchen_init("/mnt/mtd/nfs/zhangjianwei/lib/data/models/yolov3_inst2cls_960_544_opencvbgr_inst.wk" \
    //             , "/mnt/mtd/nfs/zhangjianwei/lib/data/models/attr_kitchen7_opencvbgr_inst.wk");

    img_dir_func();
    
}
// void test_nnie_kitchen()
// {
//     //SAMPLE_SVP_NNIE_Cnn();
//     SAMPLE_SVP_NNIE_Yolov3();

// }
int main(int argc, char *argv[])
{
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
