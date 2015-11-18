cmake_minimum_required(VERSION 3.2)
project(jsoncpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11 -fPIC")

set(SOURCE_FILES
        include/json/autolink.h
        include/json/config.h
        include/json/features.h
        include/json/forwards.h
        include/json/json.h
        include/json/reader.h
        include/json/value.h
        include/json/writer.h
        src/lib_json/json_internalarray.inl
        src/lib_json/json_internalmap.inl
        src/lib_json/json_valueiterator.inl
        src/lib_json/json_reader.cpp
        src/lib_json/json_value.cpp
        src/lib_json/json_writer.cpp
        src/lib_json/json_batchallocator.h)

add_library(jsoncpp SHARED ${SOURCE_FILES})
include_directories(include)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
# set_target_properties(jsoncpp PROPERTIES OUTPUT_NAME jsoncpp64)

install(TARGETS jsoncpp
        LIBRARY DESTINATION /usr/lib)
install(DIRECTORY include/json
        DESTINATION /usr/include)