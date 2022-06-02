cmake_minimum_required(VERSION 3.21)
project(EES)

set(CMAKE_CXX_STANDARD 14)
# Enable C++11

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(.)

add_executable(EES
        colordetection.cpp
        colordetection.h)

target_link_libraries(EES ${OpenCV_LIBS})