export CUR_PATH=`pwd`
export LD_LIBRARY_PATH=${CUR_PATH}/../../third_party/opencv4.4/lib/:${CUR_PATH}/../../third_party/hisi/lib/:${CUR_PATH}/build
/mnt/mtd/clean_vb
LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ${CUR_PATH}/build/test_gmm2