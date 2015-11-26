How to build using CMake

0. Dependent Libraries
   openssl
   curl

1. Create directory "build" along with this file.
   mkdir build

2. Build and install jsoncpp and spdlog
   1) install spdlog
   cd libs/spdlog
   mkdir build
   cd build
   cmake ..
   make
   sudo make install

   2) install jsoncpp
   cd libs/jsoncpp-src-0.5.0
   mkdir build
   cd build
   cmake ..
   make
   sudo make install


3. Build with CMake
   cd build
   cmake ..
   make
   sudo make install

4. sudo cp librocketmq64.so /usr/lib
   sudo mkdir /usr/include/rocketmq64
   sudo cp ../include/*.h /usr/include/rocketmq64/