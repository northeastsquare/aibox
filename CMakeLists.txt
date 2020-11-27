cmake_minimum_required(VERSION 3.10)
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
SET(THIRD_PART ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
message("3d:" ${THIRD_PART})

# Hisi
SET(HI_ROOT ${THIRD_PART}/hisi)
SET(HI_INCLUDE ${HI_ROOT}/include)
SET(HI_LIB_DIR ${HI_ROOT}/lib)

# vpu
set(HI_VPU_DIR ${THIRD_PART}/hisi_vpu)

# opencv
SET(OPENCV_ROOT ${THIRD_PART}/opencv3.4)
SET(OPENCV_INCLUDE ${OPENCV_ROOT}/include)
SET(OPENCV_LIB_DIR ${OPENCV_ROOT}/lib)
#FILE(GLOB OPENCV_LIBS "${OPENCV_LIB_DIR}/*")
#message("HI_LIB_DIR:" ${OPENCV_LIB_DIR}, ${HI_LIB_DIR})
FILE(GLOB OPENCV_LIBS "${OPENCV_LIB_DIR}/*")

INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE} ${HI_INCLUDE} interface svp svp/nnie/inc svp/nnie/detector common)
INCLUDE_DIRECTORIES(de_encoder/include )
LINK_DIRECTORIES( ${OPENCV_LIB_DIR} ${HI_LIB_DIR} ${HI_VPU_DIR})


# ------------ lib ----------------
LINK_LIBRARIES(mpi.a)
SET(HI_LIBS _hiawb.a _hiae.a _hidehaze.a upvqe.a dnvqe.a md.a
        _hildci.a _hidrc.a securec.a nnie.a ive.a tde.a VoiceEngine.a isp.a )


# --------------de-encoder---------
add_subdirectory(de_encoder)
#add_library(aibox SHARED  ${KITCHEN_SRC} $<TARGET_OBJECTS:de_encoder> $<TARGET_OBJECTS:svp>)

SET(SRC interface/svp_interface.cpp svp/nnie/detector/YoloCls.cpp svp/nnie/detector/GeneralSeg.cpp
        svp/nnie/src/Net.cpp   svp/nnie/src/nnie_core.c    svp/nnie/src/sample_comm_nnie.c  svp/nnie/src/sample_comm_svp.c)
add_library(ai_npu SHARED  ${SRC} $<TARGET_OBJECTS:de_encoder>)



ADD_EXECUTABLE(test_ai_npu tests/test_vpu_svp_multithread.cpp)
target_link_libraries(test_ai_npu  ai_npu ai_vpu
        hdmi
        mpi nnie ive dnvqe upvqe VoiceEngine securec pthread
        opencv_core opencv_imgproc opencv_imgcodecs
        stdc++ ${OPENCV_LIBS}
)
install(TARGETS ai_npu DESTINATION bin)
install(TARGETS test_ai_npu RUNTIME DESTINATION bin)

