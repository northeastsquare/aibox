#include "ai_sdk.h"

int ai_sdk_init(void)
{
    if (AI_SYS_Init() || AI_VDEC_Init() || AI_VPSS_Init() || AI_VENC_Init()) {
        return -1;
    }

    return 0;
}

int ai_sdk_exit(void)
{
    if (AI_SYS_Exit()) {
        return -1;
    }

    return 0;
}

int ai_sdk_create(AI_CHN_INFO_S *pstChnInfo)
{
    switch (pstChnInfo->enType) {
        case AI_CHN_TYPE_JPEGD:
            if (AI_VDEC_CreateChannel(pstChnInfo->s32Chn, PT_JPEG, AI_VDEC_MAX_WIDTH * 2, AI_VDEC_MAX_HEIGHT * 2)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_VPSS:
            if (AI_VDEC_CreateChannel(pstChnInfo->s32Chn, PT_JPEG, AI_VDEC_MAX_WIDTH, AI_VDEC_MAX_HEIGHT)) {
                return -1;
            }
            if (AI_VPSS_CreateChannel(pstChnInfo->s32Chn, AI_VPSS_MAX_WIDTH, AI_VPSS_MAX_HEIGHT, pstChnInfo->u32VpssWidth, pstChnInfo->u32VpssHeight, &pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VDEC_Bind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_JPEGE:
            if (AI_VDEC_CreateChannel(pstChnInfo->s32Chn, PT_JPEG, AI_VDEC_MAX_WIDTH, AI_VDEC_MAX_HEIGHT)) {
                return -1;
            }
            if (AI_VENC_CreateChannel(pstChnInfo->s32Chn, AI_VENC_MAX_WIDTH, AI_VENC_MAX_HEIGHT, pstChnInfo->u32VencWidth, pstChnInfo->u32VencHeight)) {
                return -1;
            }
            if (AI_VDEC_Bind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_VPSS_JPEGE:
            if (AI_VDEC_CreateChannel(pstChnInfo->s32Chn, PT_JPEG, AI_VDEC_MAX_WIDTH, AI_VDEC_MAX_HEIGHT)) {
                return -1;
            }
            if (AI_VPSS_CreateChannel(pstChnInfo->s32Chn, AI_VPSS_MAX_WIDTH, AI_VPSS_MAX_HEIGHT, pstChnInfo->u32VpssWidth, pstChnInfo->u32VpssHeight, &pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VENC_CreateChannel(pstChnInfo->s32Chn, AI_VENC_MAX_WIDTH, AI_VENC_MAX_HEIGHT, pstChnInfo->u32VencWidth, pstChnInfo->u32VencHeight)) {
                return -1;
            }
            if (AI_VDEC_Bind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_Bind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_VPSS:
            if (AI_VPSS_CreateChannel(pstChnInfo->s32Chn, AI_VPSS_MAX_WIDTH, AI_VPSS_MAX_HEIGHT, pstChnInfo->u32VpssWidth, pstChnInfo->u32VpssHeight, &pstChnInfo->hVbPool)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_VPSS_JPEGE:
            if (AI_VPSS_CreateChannel(pstChnInfo->s32Chn, AI_VPSS_MAX_WIDTH, AI_VPSS_MAX_HEIGHT, pstChnInfo->u32VpssWidth, pstChnInfo->u32VpssHeight, &pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VENC_CreateChannel(pstChnInfo->s32Chn, AI_VENC_MAX_WIDTH, AI_VENC_MAX_HEIGHT, pstChnInfo->u32VencWidth, pstChnInfo->u32VencHeight)) {
                return -1;
            }
            if (AI_VPSS_Bind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGE:
            if (AI_VENC_CreateChannel(pstChnInfo->s32Chn, AI_VENC_MAX_WIDTH, AI_VENC_MAX_HEIGHT, pstChnInfo->u32VencWidth, pstChnInfo->u32VencHeight)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_H264D_VPSS:
            if (AI_VDEC_CreateChannel(pstChnInfo->s32Chn, PT_H264, pstChnInfo->u32Width, pstChnInfo->u32Height)) {
                return -1;
            }
            if (AI_VPSS_CreateChannel(pstChnInfo->s32Chn, pstChnInfo->u32Width, pstChnInfo->u32Height, pstChnInfo->u32VpssWidth, pstChnInfo->u32VpssHeight, &pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VDEC_Bind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_H264D_VPSS_JPEGE:
            if (AI_VDEC_CreateChannel(pstChnInfo->s32Chn, PT_H264, AI_VDEC_MAX_WIDTH, AI_VDEC_MAX_HEIGHT)) {
                return -1;
            }
            if (AI_VPSS_CreateChannel(pstChnInfo->s32Chn, AI_VPSS_MAX_WIDTH, AI_VPSS_MAX_HEIGHT, pstChnInfo->u32VpssWidth, pstChnInfo->u32VpssHeight, &pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VENC_CreateChannel(pstChnInfo->s32Chn, AI_VENC_MAX_WIDTH, AI_VENC_MAX_HEIGHT, pstChnInfo->u32VencWidth, pstChnInfo->u32VencHeight)) {
                return -1;
            }
            if (AI_VDEC_Bind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_Bind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        default:
            break;
    }

    return 0;
}

int ai_sdk_destroy(AI_CHN_INFO_S *pstChnInfo)
{
    printf("call ai_sdk_destroy\n");
    switch (pstChnInfo->enType) {
        case AI_CHN_TYPE_JPEGD:
            if (AI_VDEC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_VPSS:
            if (AI_VDEC_UnBind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_DestroyChannel(pstChnInfo->s32Chn, pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VDEC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_JPEGE:
            if (AI_VDEC_UnBind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VENC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VDEC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_VPSS_JPEGE:
            if (AI_VPSS_UnBind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VDEC_UnBind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VENC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_DestroyChannel(pstChnInfo->s32Chn, pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VDEC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_VPSS:
            if (AI_VPSS_DestroyChannel(pstChnInfo->s32Chn, pstChnInfo->hVbPool)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_VPSS_JPEGE:
            if (AI_VPSS_UnBind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VENC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_DestroyChannel(pstChnInfo->s32Chn, pstChnInfo->hVbPool)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGE:
            if (AI_VENC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_H264D_VPSS:
            if (AI_VDEC_UnBind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_DestroyChannel(pstChnInfo->s32Chn, pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VDEC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_H264D_VPSS_JPEGE:
            if (AI_VPSS_UnBind_VENC(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VDEC_UnBind_VPSS(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VENC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            if (AI_VPSS_DestroyChannel(pstChnInfo->s32Chn, pstChnInfo->hVbPool)) {
                return -1;
            }
            if (AI_VDEC_DestroyChannel(pstChnInfo->s32Chn)) {
                return -1;
            }
            break;
        default:
            break;
    }

    return 0;
}

int ai_sdk_send(AI_CHN_INFO_S *pstChnInfo)
{
    switch (pstChnInfo->enType) {
        case AI_CHN_TYPE_JPEGD:
        case AI_CHN_TYPE_JPEGD_VPSS:
        case AI_CHN_TYPE_JPEGD_JPEGE:
        case AI_CHN_TYPE_JPEGD_VPSS_JPEGE:
        case AI_CHN_TYPE_H264D_VPSS:
        case AI_CHN_TYPE_H264D_VPSS_JPEGE:
            if (AI_VDEC_SendStream(pstChnInfo->s32Chn, pstChnInfo->pStream, pstChnInfo->u32StreamLen)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_VPSS:
        case AI_CHN_TYPE_VPSS_JPEGE:
            if (AI_VPSS_SendFrame(pstChnInfo->s32Chn, pstChnInfo->pstVFrame)) {
                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGE:
            if (AI_VENC_SendFrame(pstChnInfo->s32Chn, pstChnInfo->pstVFrame)) {
                return -1;
            }
            break;
        default:
            break;
    }

    return 0;
}

int ai_sdk_get(AI_CHN_INFO_S *pstChnInfo)
{
    HI_U64 u64PhyAddr = 0;
    HI_U32 u32Size = 0;
    HI_VOID *pVirAddr = NULL;

    switch (pstChnInfo->enType) {
        case AI_CHN_TYPE_JPEGD:
            pstChnInfo->pstVFrame = malloc(sizeof(VIDEO_FRAME_INFO_S));
            if (NULL == pstChnInfo->pstVFrame) {
                perror("malloc()");

                return -1;
            }

            if (AI_VDEC_GetFrame(pstChnInfo->s32Chn, pstChnInfo->pstVFrame)) {
                free(pstChnInfo->pstVFrame);
                pstChnInfo->pstVFrame = NULL;

                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_VPSS:
        case AI_CHN_TYPE_VPSS:
        case AI_CHN_TYPE_H264D_VPSS:
            pstChnInfo->pstVFrame = malloc(sizeof(VIDEO_FRAME_INFO_S));
            if (NULL == pstChnInfo->pstVFrame) {
                perror("malloc()");

                return -1;
            }

            if (AI_VPSS_GetChnFrame(pstChnInfo->s32Chn, pstChnInfo->pstVFrame)) {
                free(pstChnInfo->pstVFrame);
                pstChnInfo->pstVFrame = NULL;

                return -1;
            }
            break;
        case AI_CHN_TYPE_JPEGD_JPEGE:
        case AI_CHN_TYPE_JPEGD_VPSS_JPEGE:
        case AI_CHN_TYPE_VPSS_JPEGE:
        case AI_CHN_TYPE_JPEGE:
        case AI_CHN_TYPE_H264D_VPSS_JPEGE:
            if (AI_VENC_GetStream(pstChnInfo->s32Chn, &pstChnInfo->pStream, &pstChnInfo->u32StreamLen)) {
                free(pstChnInfo->pStream);
                pstChnInfo->pStream = NULL;

                return -1;
            }
            break;
        default:
            break;
    }

    return 0;
}

int ai_sdk_write_jpeg_file(char *outputFile, AI_CHN_INFO_S *pstChnInfo)
{
    FILE *pFile = NULL;

    pFile = fopen(outputFile, "wb");
    if (NULL == pFile) {
        perror("fopen()");

        return -1;
    }

    fwrite(pstChnInfo->pStream, 1, pstChnInfo->u32StreamLen, pFile);

    fclose(pFile);

    return 0;
}


int ai_sdk_release(AI_CHN_INFO_S *pstChnInfo)
{
    switch (pstChnInfo->enType) {
        case AI_CHN_TYPE_JPEGD:
            AI_VDEC_ReleaseFrame(pstChnInfo->s32Chn, pstChnInfo->pstVFrame);

            free(pstChnInfo->pstVFrame);
            pstChnInfo->pstVFrame = NULL;
            break;
        case AI_CHN_TYPE_JPEGD_VPSS:
        case AI_CHN_TYPE_VPSS:
        case AI_CHN_TYPE_H264D_VPSS:
            AI_VPSS_ReleaseChnFrame(pstChnInfo->s32Chn, pstChnInfo->pstVFrame);

            free(pstChnInfo->pstVFrame);
            pstChnInfo->pstVFrame = NULL;
            break;
        case AI_CHN_TYPE_JPEGD_JPEGE:
        case AI_CHN_TYPE_JPEGD_VPSS_JPEGE:
        case AI_CHN_TYPE_VPSS_JPEGE:
        case AI_CHN_TYPE_JPEGE:
        case AI_CHN_TYPE_H264D_VPSS_JPEGE:
            free(pstChnInfo->pStream);
            pstChnInfo->pStream = NULL;
            break;
        default:
            break;
    }

    return 0;
}
