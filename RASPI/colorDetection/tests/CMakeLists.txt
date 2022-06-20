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


include_directories(..)

add_executable(EES
        test_colordetection.cpp
        test_colordetection.h test_colordetection.cpp
        ../colordetection.h ../colordetection.cpp)
        #./Controller/controller.cpp
        #./Controller/controller.h ./Controller/controller.cpp)

add_test(NAME ColorDectection_Test COMMAND EES)

target_link_libraries(EES ${OpenCV_LIBS})
add_library(.. ../colordetection.cpp)
target_include_directories(.. PUBLIC include)




