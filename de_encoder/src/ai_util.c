#include "ai_util.h"

typedef struct BIT_STREAM
{
    HI_U8 *pu8Stream;
    HI_U32 u32BitIndex;
} BIT_STREAM_S;

HI_S32 AI_Util_GetJpegInfo(HI_U8 *pu8Data, HI_U32 u32DataLen, HI_U32 *pu32Width, HI_U32 *pu32Height)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 i = 0;

    if (pu8Data == NULL) {
        printf("AI_Util_GetJpegInfo: Data is NULL!\n");

        return HI_FAILURE;
    }

    if (pu8Data[0] != 0xFF || pu8Data[1] != 0xD8) {
        printf("AI_Util_GetJpegInfo: Start of Image is Error!\n");

        return HI_FAILURE;
    }

    if (pu8Data[u32DataLen - 2] != 0xFF || pu8Data[u32DataLen - 1] != 0xD9) {
        printf("AI_Util_GetJpegInfo: End of Image is Error!\n");

        return HI_FAILURE;
    }

    for (i = 2; i < u32DataLen - 8; i++) {
        if (pu8Data[i] == 0xFF && (pu8Data[i + 1] & 0xE0U) == 0xE0) {
            i += (pu8Data[i + 2] << 8U) + pu8Data[i + 3] + 1;
        } else if (pu8Data[i] == 0xFF && pu8Data[i + 1] == 0xC0) {
            *pu32Height = (pu8Data[i + 5] << 8U) + pu8Data[i + 6];
            *pu32Width = (pu8Data[i + 7] << 8U) + pu8Data[i + 8];
            s32Ret = HI_SUCCESS;
            break;
        }
    }

    return s32Ret;
}

static inline HI_U32 BS_GET_BIT(BIT_STREAM_S *pstBitStream)
{
    HI_U32 u32Value = 0U;

    u32Value = (HI_U32) (pstBitStream->pu8Stream[pstBitStream->u32BitIndex >> 3U] >> (7U - (pstBitStream->u32BitIndex & 7U))) & 1U;
    pstBitStream->u32BitIndex++;

    return u32Value;
}

static inline HI_U32 BS_GET_BITS(BIT_STREAM_S *pstBitStream, HI_U32 u32Num)
{
    HI_U32 u32Value = 0U;

    while (u32Num--) {
        u32Value = u32Value | (HI_U32)(BS_GET_BIT(pstBitStream) << u32Num);
    }

    return u32Value;
}

static inline HI_U32 BS_GET_UE(BIT_STREAM_S *pstBitStream)
{
    HI_U32 u32Value = 0U;
    HI_U32 u32Cnt = 0U;

    while (!BS_GET_BIT(pstBitStream)) {
        u32Cnt++;
    }

    if (u32Cnt) {
        u32Value = (1U << u32Cnt) - 1U + BS_GET_BITS(pstBitStream, u32Cnt);
    } else {
        u32Value = 0;
    }

    return u32Value;
}

static inline HI_S32 BS_GET_SE(BIT_STREAM_S *pstBitStream)
{
    HI_S32 s32Value = 0;
    HI_U32 u32Value = 0;

    u32Value = BS_GET_UE(pstBitStream) + 1;

    if (u32Value & 1U) {
        s32Value = -(u32Value >> 1U);
    } else {
        s32Value = u32Value >> 1U;
    }

    return s32Value;
}

static inline HI_VOID BS_SKIP_BIT(BIT_STREAM_S *pstBitStream)
{
    pstBitStream->u32BitIndex++;
}

static inline HI_VOID BS_SKIP_BITS(BIT_STREAM_S *pstBitStream, HI_U8 u8Num)
{
    pstBitStream->u32BitIndex += u8Num;
}

static inline HI_VOID BS_SKIP_UE(BIT_STREAM_S *pstBitStream)
{
    HI_U32 u32Cnt = 0U;

    while (!BS_GET_BIT(pstBitStream)) {
        u32Cnt++;
    }

    BS_SKIP_BITS(pstBitStream, u32Cnt);
}

static inline HI_VOID BS_SKIP_SE(BIT_STREAM_S *pstBitStream)
{
    HI_U32 u32Cnt = 0U;

    while (!BS_GET_BIT(pstBitStream)) {
        u32Cnt++;
    }

    BS_SKIP_BITS(pstBitStream, u32Cnt);
}

HI_S32 AI_Util_GetH264Info(HI_U8 *pu8Data, HI_U32 u32DataLen, HI_U32 *pu32Width, HI_U32 *pu32Height)
{
    BIT_STREAM_S stBitStream;
    HI_S32 s32Ret = HI_FAILURE;

    if (pu8Data[0] != 0x0 || pu8Data[1] != 0x0 || pu8Data[2] != 0x0
        || pu8Data[3] != 0x1 || pu8Data[4] != 0x67) {
        return HI_FAILURE;
    }

    stBitStream.pu8Stream = pu8Data + 5;
    stBitStream.u32BitIndex = 0;

    HI_U32 profile_idc = 0U;
    HI_U32 chroma_format_idc = 0U;
    HI_U32 seq_scaling_matrix_present_flag = 0U;
    HI_U32 seq_scaling_list_present_flag = 0U;
    HI_U32 pic_order_cnt_type = 0U;
    HI_U32 num_ref_frames_in_pic_order_cnt_cycle = 0U;
    HI_U32 pic_width_in_mbs_minus1 = 0U;
    HI_U32 pic_height_in_map_units_minus1 = 0U;
    HI_U32 frame_mbs_only_flag = 0U;
    HI_U32 frame_cropping_flag = 0U;
    HI_U32 frame_crop_left_offset = 0U;
    HI_U32 frame_crop_right_offset = 0U;
    HI_U32 frame_crop_top_offset = 0U;
    HI_U32 frame_crop_bottom_offset = 0U;

    profile_idc = BS_GET_BITS(&stBitStream, 8); // profile_idc: u(8)
    BS_SKIP_BITS(&stBitStream, 8); // constraint_set*_flag: u(1) * 6 + u(2)
    BS_SKIP_BITS(&stBitStream, 8); // level_idc: u(8)
    BS_SKIP_UE(&stBitStream); // seq_parameter_set_id: ue(v)
    if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 || profile_idc == 44
        || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128 || profile_idc == 138
        || profile_idc == 139 || profile_idc == 134 || profile_idc == 135) {
        chroma_format_idc = BS_GET_UE(&stBitStream); // chroma_format_idc: ue(v)
        if (chroma_format_idc == 3) {
            BS_SKIP_BIT(&stBitStream); // seq_parameter_set_id: u(1)
        }
        BS_SKIP_UE(&stBitStream); // bit_depth_luma_minus8: ue(v)
        BS_SKIP_UE(&stBitStream); // bit_depth_chroma_minus8: ue(v)
        BS_SKIP_BIT(&stBitStream); // qpprime_y_zero_transform_bypass_flag: u(1)
        seq_scaling_matrix_present_flag = BS_GET_BIT(&stBitStream); // seq_scaling_matrix_present_flag: u(1)
        if (seq_scaling_matrix_present_flag) {
            for (int i = 0; i < (chroma_format_idc != 3 ? 8 : 12); i++) {
                seq_scaling_list_present_flag = BS_GET_BIT(&stBitStream); // seq_scaling_list_present_flag: u(1)
                if (seq_scaling_list_present_flag) {
                    HI_S32 s32ScaleSize = i < 6 ? 16 : 64;
                    HI_S32 s32LastScale = 8;
                    HI_S32 s32NextScale = 8;
                    for (int j = 0; j < s32ScaleSize; j++) {
                        if (s32NextScale) {
                            s32NextScale = (HI_S32) ((HI_U32) (s32LastScale + BS_GET_SE(&stBitStream)) & 0xFFU);
                        }
                        s32LastScale = s32NextScale ? s32NextScale : s32LastScale;
                    }
                }
            }
        }
    }
    BS_SKIP_UE(&stBitStream); // log2_max_frame_num_minus4: ue(v)
    pic_order_cnt_type = BS_GET_UE(&stBitStream); // pic_order_cnt_type: ue(v)
    if (pic_order_cnt_type == 0) {
        BS_SKIP_UE(&stBitStream); // log2_max_pic_order_cnt_lsb_minus4: ue(v)
    } else if (pic_order_cnt_type == 1) {
        BS_SKIP_BIT(&stBitStream); // delta_pic_order_always_zero_flag: u(1)
        BS_SKIP_SE(&stBitStream); // offset_for_non_ref_pic: se(v)
        BS_SKIP_SE(&stBitStream); // offset_for_top_to_bottom_field: se(v)
        num_ref_frames_in_pic_order_cnt_cycle = BS_GET_UE(&stBitStream); // num_ref_frames_in_pic_order_cnt_cycle: ue(v)
        for (int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++) {
            BS_SKIP_SE(&stBitStream); // offset_for_ref_frame: se(v)
        }
    }
    BS_SKIP_UE(&stBitStream); // max_num_ref_frames: ue(v)
    BS_SKIP_BIT(&stBitStream); // gaps_in_frame_num_value_allowed_flag: u(1)
    pic_width_in_mbs_minus1 = BS_GET_UE(&stBitStream); // pic_width_in_mbs_minus1: ue(v)
    pic_height_in_map_units_minus1 = BS_GET_UE(&stBitStream); // pic_height_in_map_units_minus1: ue(v)
    frame_mbs_only_flag = BS_GET_BIT(&stBitStream); // frame_mbs_only_flag: u(1)
    if (!frame_mbs_only_flag) {
        BS_SKIP_BIT(&stBitStream); // mb_adaptive_frame_field_flag: u(1)
    }
    BS_SKIP_BIT(&stBitStream); // direct_8x8_inference_flag: u(1)
    frame_cropping_flag = BS_GET_BIT(&stBitStream); // frame_cropping_flag: u(1)
    if (frame_cropping_flag) {
        frame_crop_left_offset = BS_GET_UE(&stBitStream); // frame_crop_left_offset: ue(v)
        frame_crop_right_offset = BS_GET_UE(&stBitStream); // frame_crop_right_offset: ue(v)
        frame_crop_top_offset = BS_GET_UE(&stBitStream); // frame_crop_top_offset: ue(v)
        frame_crop_bottom_offset = BS_GET_UE(&stBitStream); // frame_crop_bottom_offset: ue(v)
    }

    *pu32Width = (pic_width_in_mbs_minus1 + 1) * 16;
    *pu32Height = (pic_height_in_map_units_minus1 + 1) * 16 * (2 - frame_mbs_only_flag);
    if (frame_cropping_flag) {
        if (chroma_format_idc == 0) {
            *pu32Width -= (frame_crop_left_offset + frame_crop_right_offset);
            *pu32Height -= (frame_crop_top_offset + frame_crop_bottom_offset) * (2 - frame_mbs_only_flag);
        } else if (chroma_format_idc == 1) {
            *pu32Width -= (frame_crop_left_offset + frame_crop_right_offset) * 2;
            *pu32Height -= (frame_crop_top_offset + frame_crop_bottom_offset) * (2 - frame_mbs_only_flag) * 2;
        } else if (chroma_format_idc == 2) {
            *pu32Width -= (frame_crop_left_offset + frame_crop_right_offset) * 2;
            *pu32Height -= (frame_crop_top_offset + frame_crop_bottom_offset) * (2 - frame_mbs_only_flag);
        } else if (chroma_format_idc == 3) {
            *pu32Width -= (frame_crop_left_offset + frame_crop_right_offset);
            *pu32Height -= (frame_crop_top_offset + frame_crop_bottom_offset) * (2 - frame_mbs_only_flag);
        }
    }

    return HI_SUCCESS;
}

HI_S32 readTemperature(HI_FLOAT *temperature){
    HI_VOID* p_MISC_CTRL28 = NULL;
	HI_VOID* p_MISC_CTRL30 = NULL;
	HI_U32 tmp = 0;

	HI_S32 dev_fd = open("/dev/mem", O_RDWR | O_SYNC);

	if (dev_fd < 0)
	{
		printf("open(/dev/mem) failed.");
		return HI_FAILURE;
	}

	HI_U8* map_base = (HI_VOID*)mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0x12030000);

	if (NULL == map_base)
	{
		printf("Memory Map error.");
		return HI_FAILURE;
	}

	p_MISC_CTRL28 = map_base + 0x0070;
	p_MISC_CTRL30 = map_base + 0x0078;
	
    *(HI_U32*)p_MISC_CTRL28 = 0x00000000;
    *(HI_U32*)p_MISC_CTRL28 = 0x80000000;

    tmp = *(HI_U32*)p_MISC_CTRL30 & 0x3ff;
    *temperature = 1.0 * (tmp - 116) / 806 * 165 - 40;
    // printf("Temprature:%f 0C\n", *temperature);

    return HI_SUCCESS;
}


HI_S32 readLocalTime(HI_U8 *pstr){
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    snprintf(pstr, 20, "%d-%02d-%02d %02d:%02d:%02d", (1900 +p->tm_year),(1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    return HI_SUCCESS;
}


HI_S32 readROM(HI_S32 *memTotal, HI_S32 *memUsed, HI_FLOAT *memRate)
{
    FILE *fstream = NULL;
    HI_U8 buff[1024];
    HI_U8 *pch1, *pch2, *pch3;
    memset(buff, 0, sizeof(buff));	
    
    if(NULL == (fstream = popen("free -k | sed -n '2p' | awk '{print \" \"$2\":\"$3\":\"$3*100/$2\":\"}'","r"))) 
    {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return HI_FAILURE;
    }

    while(NULL != fgets(buff, sizeof(buff), fstream))
    {
            //printf("%s",buff);
    }

    pch1=strtok(buff, ":");
    *memTotal = atoi(pch1);
    pch2=strtok(NULL,":");
    *memUsed = atoi(pch2);
    pch3=strtok(NULL,":");
    *memRate = atof(pch3);
    pclose(fstream);

    return HI_SUCCESS;
}


HI_S32 readHdSizeUsed(HI_S32 *hdSizeUsed)
{
#if 0
    FILE *fstream = NULL;
    HI_U8 buff[1024];
    HI_U8 *pch1;
    memset(buff, 0, sizeof(buff));

    if(NULL == (fstream = popen("/mnt/mtd/ai/bin/du -sm / 2>/dev/null","r")))
    {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return HI_FAILURE;
    }

    while(NULL != fgets(buff, sizeof(buff), fstream))
    {
           //printf("%s",buff);
    }

    pch1=strtok(buff, " ");
    *hdSizeUsed = atoi(pch1);
#endif
    *hdSizeUsed = 0;

    return HI_SUCCESS;
}


HI_S32 readCpuUsed(HI_FLOAT *cpuUsed )
{
    FILE *fstream = NULL;
    HI_U8 buff[1024];
    HI_U8 *pch1,*pch2;
    memset(buff, 0, sizeof(buff));	
    
    if(NULL == (fstream = popen("top -n 1| grep \"Cpu(s)\" | awk \'{print $2,$4}\'","r"))) 
    {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return HI_FAILURE;
    }

    while(NULL != fgets(buff, sizeof(buff), fstream))
    {
        //    printf("%s",buff);
    }

    pch1=strtok(buff, " ");
    pch2=strtok(NULL," ");
       
    // printf("s1: %s, s2: %s",pch1, pch2);
    *cpuUsed = atof(pch1)+ atof(pch2);

    return HI_SUCCESS;
}


HI_S32 readSysInfo(SYS_INFO_S *sysinfo){
    HI_FLOAT temperature;
    HI_S32 memUsed, memTotal;
    HI_FLOAT memRate;
    HI_S32 hdSizeUsed;
    HI_FLOAT cpuUsed;

    if (readTemperature(&temperature) || readROM(&memTotal, &memUsed, &memRate) || readHdSizeUsed(&hdSizeUsed) || readCpuUsed(&cpuUsed))
    {
        printf("Can not read all info.\n");
        return HI_FAILURE;
    }
    printf("temperature: %.2f C.\n", temperature);
    printf("memery total: %d KB; memery used: %d KB; memery used rate: %.2f%%.\n", memTotal, memUsed, memRate);
    printf("hard disk size used: %d MB.\n", hdSizeUsed);
    printf("cpu used : %.2f%%\n", cpuUsed);

    sysinfo->temperature = temperature;
    sysinfo->memTotal = memTotal;
    sysinfo->memUsed = memUsed;
    sysinfo->memRate = memRate;
    sysinfo->hdSizeUsed = hdSizeUsed;
    sysinfo->cpuUsed = cpuUsed;

    return HI_SUCCESS;
}

