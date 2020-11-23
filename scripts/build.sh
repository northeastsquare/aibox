mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=. ..
make -j8
make install