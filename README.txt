How to build using CMake

1. Create directory "build" along with this file.
   mkdir build

2. Build with CMake
   cd build
   cmake ..
   make

3. sudo cp librocketmq64.so /usr/lib
   sudo mkdir /usr/include/rocketmq64
   sudo cp ../include/*.h /usr/include/rocketmq64/