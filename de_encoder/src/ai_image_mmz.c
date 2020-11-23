#include <stdlib.h>
#include <string.h>

#include <hi_type.h>
#include <mpi_sys.h>

#include "ai_image_mmz.h"

int ai_malloc_image_mmz_cached(AI_IMAGE_S **image, HI_U32 width, HI_U32 height)
{
    printf("into mmz\r\n");
    AI_IMAGE_S  *p_image = NULL;
    HI_U32      size = 0;
    HI_U64      phy_addr = 0;
    HI_U8       *vir_addr = NULL;

    p_image = malloc(sizeof(AI_IMAGE_S));
    memset(p_image, 0, sizeof(AI_IMAGE_S));

    size = width * height * 3;
     printf("1 mmz\r\n");
    HI_MPI_SYS_MmzAlloc_Cached(&phy_addr, (HI_VOID **)&vir_addr, "ai_image_mmz_cached", NULL, size);
      printf("2 mmz\r\n");   
    p_image->phy_addr[0] = (void *)(HI_U64)phy_addr;
    p_image->phy_addr[1] = (void *)(HI_U64)(phy_addr + width * height);
    p_image->phy_addr[2] = (void *)(HI_U64)(phy_addr + width * height * 2);

    p_image->vir_addr[0] = (void *)vir_addr;
    p_image->vir_addr[1] = (void *)(vir_addr + width * height);
    p_image->vir_addr[2] = (void *)(vir_addr + width * height * 2);

    p_image->stride[0]   = width ;
    p_image->stride[1]   = width ;
    p_image->stride[2]   = width ;

    p_image->width       = width;
    p_image->height      = height;

    p_image->channel     = 0;

    *image = p_image;
     printf("outto mmz\r\n");
    return 0;
}

int ai_flush_image_mmz_cache(AI_IMAGE_S *image)
{
    HI_U64      phy_addr = 0;
    HI_U8       *vir_addr = NULL;
    HI_U32      size = 0;

    phy_addr = (HI_U64)image->phy_addr[0];
    vir_addr = image->vir_addr[0];
    size = image->width * image-> height * 3;

    HI_MPI_SYS_MmzFlushCache(phy_addr, (HI_VOID *)vir_addr, size);

    return 0;
}



int ai_release_image_mmz_cached(AI_IMAGE_S *image)
{
    HI_U64      phy_addr = 0;
    HI_U8       *vir_addr = NULL;

    phy_addr = (HI_U64)image->phy_addr[0];
    vir_addr = image->vir_addr[0];

    HI_MPI_SYS_MmzFree(phy_addr, (HI_VOID *)vir_addr);

    free(image);

    return 0;
}
