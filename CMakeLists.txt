cmake_minimum_required(VERSION 3.10)
project(EES)
include(CTest)

enable_testing()

set(OpenCV_DIR "~/opencv/build")

set(CMAKE_CXX_STANDARD 14)
# Enable C++11
set(/build "path_to_directory_of_your_exe")

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(colorDetection)

add_executable(EES
        ./colorDetection/tests/test_colordetection.cpp
        ./colorDetection/tests/test_colordetection.h ./colorDetection/tests/test_colordetection.cpp
        ./colorDetection/colordetection.h ./colorDetection/colordetection.cpp)
        #./Controller/controller.cpp
        #./Controller/controller.h ./Controller/controller.cpp)

add_test(NAME ColorDectection_Test COMMAND EES)

target_link_libraries(EES ${OpenCV_LIBS})
add_library(colorDetection ./colorDetection/colordetection.cpp)
target_include_directories(colorDetection PUBLIC include)




