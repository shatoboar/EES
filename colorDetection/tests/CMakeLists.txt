cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

enable_testing()

add_executable(
        test_colordetection
        test_colordetection.cpp
        test_colordetection.h
)
target_link_libraries(
        test_colordetection
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_colordetection)