include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)
find_package(GTest REQUIRED)
add_executable(test_colordetection test_colordetection.cpp)

target_link_libraries(test_colordetection
        PRIVATE
        GTest::GTest
        colordetection)

add_test(tests test_colordetection)