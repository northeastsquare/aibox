
export CUR_PATH=`pwd`
export LD_LIBRARY_PATH=${CUR_PATH}/third_party/opencv4.4/lib/:${CUR_PATH}/third_party/hisi/lib/:${CUR_PATH}/build/bin
/mnt/mtd/clean_vb
LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ${CUR_PATH}/build/bin/test_kitchen
#LD_LIBRARY_PATH=${LD_LIBRARY_PATH} ${CUR_PATH}/build/bin/nnie_kitchen_test
