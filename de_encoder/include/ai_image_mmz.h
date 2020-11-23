#ifndef AIBOX_SDK_AI_MMZIMAGE_H
#define AIBOX_SDK_AI_MMZIMAGE_H

#include <hi_type.h>

#include "ai_common.h"

#ifdef __cplusplus
extern "C" {
#endif

int ai_malloc_image_mmz_cached(AI_IMAGE_S **image, HI_U32 width, HI_U32 height);

int ai_flush_image_mmz_cache(AI_IMAGE_S *image);

int ai_release_image_mmz_cached(AI_IMAGE_S *image);

#ifdef __cplusplus
}
#endif

#endif //AIBOX_SDK_AI_MMZIMAGE_H
