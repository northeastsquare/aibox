cmake_minimum_required(VERSION 3.10)
project(aibox_sdk C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

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
message("3d:" ${THIRD_PART})

# Hisi
SET(HI_ROOT ${THIRD_PART}/hisi)
SET(HI_INCLUDE ${HI_ROOT}/include)
SET(HI_LIB_DIR ${HI_ROOT}/lib)
# opencv
SET(OPENCV_ROOT ${THIRD_PART}/opencv4.4)
SET(OPENCV_INCLUDE ${OPENCV_ROOT}/include/opencv4/)
SET(OPENCV_LIB_DIR ${OPENCV_ROOT}/lib)
FILE(GLOB OPENCV_LIBS "${OPENCV_LIB_DIR}/*")
message("HI_LIB_DIR:" ${OPENCV_LIB_DIR}, ${HI_LIB_DIR})
INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE} ${HI_INCLUDE} nnie/inc nnie/detector ../de_encoder/include/)
LINK_DIRECTORIES( ${OPENCV_LIB_DIR} ${HI_LIB_DIR})


# ------------ lib ----------------
LINK_LIBRARIES(mpi.a)
SET(HI_LIBS _hiawb.a _hiae.a _hidehaze.a upvqe.a dnvqe.a md.a
        _hildci.a _hidrc.a securec.a nnie.a ive.a tde.a VoiceEngine.a isp.a )

#INCLUDE_DIRECTORIES(inc ../de_encoder/include/  ${OPENCV_INCLUDE} ${HI_INCLUDE})
#LINK_DIRECTORIES( ${OPENCV_LIB_DIR} ${HI_LIB_DIR})

FILE(GLOB SRC nnie/detector/*.cpp  nnie/src/*.c nnie/src/*.cpp ive/*.c ../common/sample_comm_sys.c)
#SET(NET_SRC ./nnie/Net.cpp)

add_library(svp OBJECT  ${SRC} )
#TARGET_LINK_LIBRARIES(nnie_kitchen  ${HI_LIBS} ${OPENCV_LIBS} -lstdc++ -lpthread -lm -ldl )
install(TARGETS svp DESTINATION bin)

# ADD_EXECUTABLE(nnie_kitchen_test ${SRC} kitchen/test_kitchen.cpp)
# TARGET_LINK_LIBRARIES(nnie_kitchen_test ${HI_LIBS} ${OPENCV_LIBS} -lstdc++ -lpthread -lm -ldl )
# install(TARGETS nnie_kitchen_test DESTINATION bin)


# For YOLO
#ADD_EXECUTABLE(alg_Exe ${SRC} examples/yolo.cpp)
#TARGET_LINK_LIBRARIES(alg_Exe  ${HI_LIBS} ${OPENCV_LIBS} -lstdc++ -lpthread -lm -ldl )


# For MNIST
#ADD_EXECUTABLE(Cls_Exe ${SRC} ./examples/GeneralCls.h ./examples/GeneralCls.cpp)
#TARGET_LINK_LIBRARIES(Cls_Exe  ${HI_LIBS} ${OPENCV_LIBS} -lstdc++ -lpthread -lm -ldl )


# For ENet
#ADD_EXECUTABLE(Seg_Exe ${SRC} ./examples/GeneralSeg.h ./examples/GeneralSeg.cpp)
#TARGET_LINK_LIBRARIES(Seg_Exe  ${HI_LIBS} ${OPENCV_LIBS} -lstdc++ -lpthread -lm -ldl )

