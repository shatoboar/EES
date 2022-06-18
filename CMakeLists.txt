cmake_minimum_required(VERSION 3.10)
project(EES)

set(OpenCV_DIR "~/opencv/build")

set(CMAKE_CXX_STANDARD 14)
# Enable C++11

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(colorDetection)

add_executable(EES
        ./colorDetection/test_colordetection.cpp
        ./colorDetection/test_colordetection.h ./colorDetection/test_colordetection.cpp)
        #./colorDetection/colordetection.cpp
        #./colorDetection/colordetection.h ./colorDetection/colordetection.cpp)
        #./Controller/controller.cpp
        #./Controller/controller.h ./Controller/controller.cpp)

target_link_libraries(EES ${OpenCV_LIBS})
