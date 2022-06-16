//
// Created by Timo Oeltze on 31.05.22.
//

#ifndef EES_COLORDETECTION_H
#define EES_COLORDETECTION_H
#include <iostream>
#include <string.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


typedef enum class Color
{red, green, blue, yellow
}Color;

typedef enum class Color_detected
{
    red, green, blue, yellow, no_object, several_colors
}Color_detected;

class ColorDetection {
public:
        Mat img;
        vector<Point> red_counter;
        vector<Point> green_counter;
        vector<Point> blue_counter;
        vector<Point> yellow_counter;
        bool red_brick = false;
        bool green_brick = false;
        bool blue_brick = false;
        bool yellow_brick = false;
        bool red_brick_only_one;
        bool green_brick_only_one;
        bool blue_brick_only_one;
        bool yellow_brick_only_one;
        Color_detected color_detection_result;
        Scalar red_lower = Scalar(136, 87, 111);
        Scalar red_upper = Scalar(180, 255, 255);
        Scalar green_lower = Scalar(36, 25, 25);
        Scalar green_upper = Scalar(76, 255, 255);
        Scalar blue_lower = Scalar(94, 80, 2);
        Scalar blue_upper = Scalar(114, 255, 255);
        Scalar yellow_lower = Scalar(15, 100, 100);
        Scalar yellow_upper = Scalar(30, 255, 255);
        ColorDetection(char* filename);
        vector<Point> get_red_counter();
        vector<Point> get_green_counter();
        vector<Point> get_blue_counter();
        vector<Point> get_yellow_counter();
};

#endif //EES_COLORDETECTION_H
