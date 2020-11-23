#include "ai_comm_rgn.h"



HI_S32 AI_RGN_ATTACH_BITMAP_TO_CHN(HI_S32 PcivChn, BITMAP_S *stBitmap,RGN_OVERLAY_PARA *param)
{
    HI_S32 s32Ret;
    MPP_CHN_S stChn;
    RGN_ATTR_S stRgnAttr;
    RGN_CHN_ATTR_S stChnAttr;
   
    /* creat region*/
    stRgnAttr.enType = OVERLAYEX_RGN;
    stRgnAttr.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_ARGB_1555;
    stRgnAttr.unAttr.stOverlayEx.stSize.u32Width  = param->WIDTH;
    stRgnAttr.unAttr.stOverlayEx.stSize.u32Height = param->HEIGHT;
    stRgnAttr.unAttr.stOverlayEx.u32BgColor = param->BgColor;
    stRgnAttr.unAttr.stOverlayEx.u32CanvasNum = 2;

    s32Ret = HI_MPI_RGN_Create(PcivChn, &stRgnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("region of pciv chn %d create fail. value=0x%x.", PcivChn, s32Ret);
        return s32Ret;
    }
    printf("rgn 1\r\n");
    /*attach region to chn*/
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = PcivChn;
    stChn.s32ChnId = 0;

    stChnAttr.bShow = HI_TRUE;
    stChnAttr.enType = OVERLAYEX_RGN;
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = param->SX;
    stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = param->SY;
    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha   = param->BgAlpha;
    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha   = param->FgAlpha;
    stChnAttr.unChnAttr.stOverlayExChn.u32Layer     = 0;

    /* load bitmap*/
    
    printf("rgn 2\r\n");
    s32Ret = HI_MPI_RGN_SetBitMap(PcivChn, stBitmap);
    if (s32Ret != HI_SUCCESS)
    {
        printf("region set bitmap to  pciv chn %d fail. value=0x%x.", PcivChn, s32Ret);
		free(stBitmap->pData);
        return s32Ret;
    }
    free(stBitmap->pData);

    s32Ret = HI_MPI_RGN_AttachToChn(PcivChn, &stChn, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("region attach to  pciv chn %d fail. value=0x%x.", PcivChn, s32Ret);
        return s32Ret;
    }
    printf("rgn 3\r\n");
    return HI_SUCCESS;
}

HI_S32 AI_RGN_Destory_Region(HI_S32 PcivChn)
{
    HI_S32 s32Ret;
    MPP_CHN_S stChn;
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = PcivChn;
    stChn.s32ChnId = 0;
    /* detach region from chn */
    s32Ret = HI_MPI_RGN_DetachFromChn(PcivChn, &stChn);
 
    if (s32Ret != HI_SUCCESS)
    {
        printf("region attach to  pciv chn %d fail. value=0x%x.", PcivChn, s32Ret);
        return s32Ret;
    }

    /* destroy region */
    s32Ret = HI_MPI_RGN_Destroy(PcivChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("destroy  pciv chn %d region fail. value=0x%x.", PcivChn, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 AI_Load_Bmp(const char *filename, BITMAP_S *pstBitmap, HI_BOOL bFil, HI_U32 u16FilColor)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;
  
    if(GetBmpInfo(filename,&bmpFileHeader,&bmpInfo) < 0)
    {
		printf("GetBmpInfo err!\n");
        return HI_FAILURE;
    }
   
    Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
  
    pstBitmap->pData = malloc(2*(bmpInfo.bmiHeader.biWidth)*(bmpInfo.bmiHeader.biHeight));

    if(NULL == pstBitmap->pData)
    {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }
  
    CreateSurfaceByBitMap(filename,&Surface,(HI_U8*)(pstBitmap->pData));

    pstBitmap->u32Width = Surface.u16Width;
    pstBitmap->u32Height = Surface.u16Height;
    pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_1555;

    int i,j;
    HI_U16 *pu16Temp;
    pu16Temp = (HI_U16*)pstBitmap->pData;

    if (bFil)
    {
        for (i=0; i<pstBitmap->u32Height; i++)
        {
            for (j=0; j<pstBitmap->u32Width; j++)
            {
                if (u16FilColor == *pu16Temp)
                {
                    *pu16Temp &= 0x7FFF;
                }
                pu16Temp++;
            }
        }

    }

    return HI_SUCCESS;
}