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
include_directories(BluetoothService)
include_directories(Controller)
add_library(Bluetooth_Service
        BluetoothService/BluetoothService.cpp
        BluetoothService/BluetoothService.h)
add_executable(EES_Controller
        Controller/controller.cpp
        BluetoothService/BluetoothService.h BluetoothService/BluetoothService.cpp
        Controller/controller.h Controller/controller.cpp
        colorDetection/colordetection.h colorDetection/colordetection.cpp)

add_executable(Color_Test
        colorDetection/tests/test_colordetection.cpp
        colorDetection/tests/test_colordetection.h colorDetection/tests/test_colordetection.cpp
        colorDetection/colordetection.h colorDetection/colordetection.cpp)

add_test(NAME ColorDectection_Test COMMAND Color_Test)

target_link_libraries(Bluetooth_Service "bluetooth")
target_link_libraries(EES_Controller ${OpenCV_LIBS})
target_link_libraries(EES_Controller "bluetooth")
target_link_libraries(Color_Test ${OpenCV_LIBS})
add_library(colorDetection colorDetection/colordetection.cpp)
target_include_directories(colorDetection PUBLIC include)




