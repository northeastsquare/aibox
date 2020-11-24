# ------------------------------- FLAGS -----------------------------
SET(CMAKE_C_COMPILER aarch64-himix100-linux-gcc)

SET(CMAKE_CXX_COMPILER aarch64-himix100-linux-g++)
SET(CMAKE_C_STRIP aarch64-himix100-linux-strip)

SET(CMAKE_C_FLAGS_PUBLIC "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -ffunction-sections -mno-unaligned-access -fno-aggressive-loop-optimizations -mapcs-frame -rdynamic")
SET(CMAKE_C_FLAGS_DEBUG "-Wall -ggdb3 -DNM_DEBUG ${CMAKE_C_FLAGS_PUBLIC}")
SET(CMAKE_C_FLAGS_RELEASE  "-Wall -O3  ${CMAKE_C_FLAGS_PUBLIC}")

add_definitions(-D__DEBUG__)
add_compile_options(-fPIC)

# --------------------------------
SET(THIRD_PART ${CMAKE_CURRENT_SOURCE_DIR}/../third_party)
#SET(THIRD_PART ../third_party)
# Hisi
SET(HI_ROOT ${THIRD_PART}/hisi)
SET(HI_INCLUDE ${HI_ROOT}/include)
SET(HI_LIB_DIR ${HI_ROOT}/lib)
# opencv
SET(OPENCV_ROOT ${THIRD_PART}/opencv4.4)
SET(OPENCV_INCLUDE ${OPENCV_ROOT}/include/opencv4/)
SET(OPENCV_LIB_DIR ${OPENCV_ROOT}/lib)
FILE(GLOB OPENCV_LIBS "${OPENCV_LIB_DIR}/*")

message("HI_LIB_DIR deencoder:" ${OPENCV_LIB_DIR}, ${HI_LIB_DIR})
INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE} ${HI_INCLUDE} include ../nnie_kitchen/kitchen/ 
                ../nnie_kitchen/nnie/detector ../nnie_kitchen/nnie/inc)
#LINK_DIRECTORIES( ${OPENCV_LIB_DIR} ${HI_LIB_DIR} ../build/nnie_kitchen/)

#LINK_LIBRARIES(mpi.a)
# SET(HI_LIBS mpi.a _hiawb.a _hiae.a _hidehaze.a upvqe.a dnvqe.a md.a
#         _hildci.a _hidrc.a securec.a nnie.a ive.a tde.a VoiceEngine.a isp.a )

file(GLOB SOURCES
     src/*.c
    src/*.cpp
)

# add_library(aibox SHARED
#          ${SOURCES}
#         )

#add_executable(sample ai_sample_multithread.cpp)
add_library(de_encoder OBJECT  ${SOURCES})

# target_link_libraries(sample
#         aibox
#         hdmi
#        mpi nnie ive dnvqe upvqe VoiceEngine securec pthread
#         opencv_core opencv_imgproc opencv_imgcodecs
#         stdc++ ${OPENCV_LIBS}
# )
install(TARGETS de_encoder  DESTINATION bin)
#install(TARGETS sample RUNTIME DESTINATION bin)
