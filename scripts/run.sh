
export CUR_PATH=`pwd`
export LD_LIBRARY_PATH=${CUR_PATH}/third_party/opencv3.4/lib/:${CUR_PATH}/third_party/hisi/lib/:${CUR_PATH}/third_party/hisi_vpu/:${CUR_PATH}/build/bin
/mnt/mtd/clean_vb
LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ${CUR_PATH}/build/bin/test_ai_npu
#LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ${CUR_PATH}/build/bin/nnie_kitchen_test
