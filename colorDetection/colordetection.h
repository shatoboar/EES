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

class ColorDetection {
public:
        Mat img;
        vector<Point> redCounter;
        vector<Point> greenCounter;
        vector<Point> blueCounter;
        vector<Point> yellowCounter;
        bool redBrick;
        bool greenBrick;
        bool blueBrick;
        bool yellowBrick;
        Scalar red_lower = Scalar(136, 87, 111);
        Scalar red_upper = Scalar(180, 255, 255);
        Scalar green_lower = Scalar(36, 25, 25);
        Scalar green_upper = Scalar(76, 255, 255);
        Scalar blue_lower = Scalar(94, 80, 2);
        Scalar blue_upper = Scalar(114, 255, 255);
        Scalar yellow_lower = Scalar(15, 100, 100);
        Scalar yellow_upper = Scalar(30, 255, 255);
        ColorDetection(char* filename);
        vector<Point> getRedCounter();
        vector<Point> getGreenCounter();
        vector<Point> getBlueCounter();
        vector<Point> getYellowCounter();
};


#endif //EES_COLORDETECTION_H
