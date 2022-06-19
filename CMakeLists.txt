cmake_minimum_required(VERSION 3.10)
project(EES)
enable_testing()

set(OpenCV_DIR "~/opencv/build")

set(CMAKE_CXX_STANDARD 14)
# Enable C++11
set(/build "path_to_directory_of_your_exe")

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(colorDetection)

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)

add_executable(EES
        ./colorDetection/test_colordetection.cpp
        ./colorDetection/colordetection.h ./colorDetection/colordetection.cpp
        ./colorDetection/test_colordetection.h ./colorDetection/test_colordetection.cpp)
        #./Controller/controller.cpp
        #./Controller/controller.h ./Controller/controller.cpp)

target_link_libraries(EES ${OpenCV_LIBS})

enable_testing()
add_executable(
        colorDetection-Test
        ./colorDetection/test_colordetection.cpp
)

target_link_libraries(
        colorDetection-Test
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(colorDetection-Test)
