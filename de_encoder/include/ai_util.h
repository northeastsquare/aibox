#ifndef AIBOX_SDK_AI_UTIL_H
#define AIBOX_SDK_AI_UTIL_H

#include "ai_common.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>

typedef struct SYS_INFO_ST
{
    HI_FLOAT temperature; // CPU温度
    HI_S32 memTotal; // RAM大小
    HI_S32 memUsed; // RAM使用
    HI_FLOAT memRate; // RAM使用率
    HI_S32 hdSizeUsed; // 已用存储
    HI_FLOAT cpuUsed; // CPU使用率
} SYS_INFO_S;

HI_S32 AI_Util_GetJpegInfo(HI_U8 *pu8Data, HI_U32 u32DataLen, HI_U32 *pu32Width, HI_U32 *pu32Height);

HI_S32 AI_Util_GetH264Info(HI_U8 *pu8Data, HI_U32 u32DataLen, HI_U32 *pu32Width, HI_U32 *pu32Height);

HI_S32 readTemperature(HI_FLOAT *temperature);
HI_S32 readLocalTime(HI_U8 *pstr);
HI_S32 readROM(HI_S32 *memTotal, HI_S32 *memUsed, HI_FLOAT *memRate);
HI_S32 readHdSizeUsed(HI_S32 *hdSizeUsed);
HI_S32 readCpuUsed(HI_FLOAT *cpuUsed );
HI_S32 readSysInfo(SYS_INFO_S *sysinfo);

#endif //AIBOX_SDK_AI_UTIL_H
