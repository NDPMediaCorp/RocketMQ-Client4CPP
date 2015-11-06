cmake_minimum_required(VERSION 3.2)

project(example_simple)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I../../include")
set(SOURCE_FILES Producer.cpp)
LINK_LIBRARIES(rocketmq64)

add_executable(simple ${SOURCE_FILES})