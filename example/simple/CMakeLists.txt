cmake_minimum_required(VERSION 3.2)

project(simple)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11")
set(CMAKE_VERBOSE_MAKEFILE ON)

set(PRODUCER_FILES Producer.cpp)

set(PUSH_CONSUMER_FILES PushConsumer.cpp)

set(PULL_CONSUMER_FILES PullConsumer.cpp)

# link against shared library in /usr/lib/librocketmq64.so
LINK_LIBRARIES(rocketmq)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(z)
LINK_LIBRARIES(jsoncpp)
LINK_LIBRARIES(ssl)
LINK_LIBRARIES(curl)

# include_directories(../../include)

add_executable(producer ${PRODUCER_FILES})

add_executable(push_consumer ${PUSH_CONSUMER_FILES})

add_executable(pull_consumer ${PULL_CONSUMER_FILES})

# target_link_libraries(rocketmq64)
# target_link_libraries(jsoncpp)