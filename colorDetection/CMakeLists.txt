cmake_minimum_required(VERSION 3.10)
project(EES)

set(OpenCV_DIR "~/opencv/build")

set(CMAKE_CXX_STANDARD 14)
# Enable C++11

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(.)

add_executable(EES
        colordetection.cpp
        colordetection.h colordetection.cpp)

target_link_libraries(EES ${OpenCV_LIBS})
