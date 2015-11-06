cmake_minimum_required(VERSION 3.2)

project(simple)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11")
set(CMAKE_VERBOSE_MAKEFILE ON)

set(SOURCE_FILES Producer.cpp)

# link against shared library in /usr/lib/librocketmq64.so
LINK_LIBRARIES(rocketmq64)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(z)
LINK_LIBRARIES(jsoncpp64)

include_directories(../../include)
add_executable(simple ${SOURCE_FILES})
# target_link_libraries(rocketmq64)
# target_link_libraries(jsoncpp)