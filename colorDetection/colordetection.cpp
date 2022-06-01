//
// Created by Timo Oeltze on 31.05.22.
//

#include "colordetection.h"
#include <iostream>
#include <string.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/**
 * returns biggest area for parameter mask or null pointer
 * @param mask
 * @return null pointer or vector
 */
vector<Point> detect_max_color(Mat mask) {
    int max_area = 0;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<cv::Point> large_contour;
    findContours( mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

    for(int i = 0; i < contours.size(); i++){
        int area = (int)cv::contourArea(contours[i]);
        if (area > max_area) {
            large_contour = contours[i];
            max_area = area;
        }
    }

    return large_contour;
}

void box_creator(Mat img, vector<Point> contour, int r, int g, int b, string name) {
    Scalar color(r,g,b);
    Rect area = boundingRect(Mat (contour));
    rectangle(img, area, color,1,8,0);
    putText(img, name, contour[0], FONT_ITALIC, 0.8, Scalar (139, 35, 35));
}

void show(Mat img, vector<Point> red, vector<Point> green, vector<Point> blue, vector<Point> yellow) {
    if (red.size() > 0) {
        box_creator(img, red, 0, 0, 255, "Red Colour");
    }
    if (green.size() > 0) {
        box_creator(img, green, 0, 255, 0, "Green Colour");
    }
    if (blue.size() > 0) {
        box_creator(img, blue, 255, 0, 0, "Blue Colour");
    }
    if (yellow.size() > 0) {
        box_creator(img, yellow, 255, 255, 255, "Yellow Colour");
    }
}

int main(int argc,char* argv[]) {
    Mat img, img_hsv;
    Mat red_mask, green_mask, blue_mask, yellow_mask;
    Mat res_red, res_green, res_blue, res_yellow;

    // read picture
    img = imread(argv[1]);

    // convert from RGB to HSV
    cvtColor(img, img_hsv, COLOR_BGR2HSV);

    // set color range red and define mask
    Scalar red_lower = Scalar(136, 87, 111);
    Scalar red_upper = Scalar(180, 255, 255);
    inRange(img_hsv, red_lower, red_upper, red_mask);

    // set color range green and define mask
    Scalar green_lower = Scalar(36, 25, 25);
    Scalar green_upper = Scalar(76, 255, 255);
    inRange(img_hsv, green_lower, green_upper, green_mask);

    // set color range blue and define mask
    Scalar blue_lower = Scalar(94, 80, 2);
    Scalar blue_upper = Scalar(114, 255, 255);
    inRange(img_hsv, blue_lower, blue_upper, blue_mask);

    // set color range yellow and define mask
    Scalar yellow_lower = Scalar(15, 100, 100);
    Scalar yellow_upper = Scalar(30, 255, 255);
    inRange(img_hsv, yellow_lower, yellow_upper, yellow_mask);

    dilate(red_mask, red_mask, CV_8U);
    dilate(green_mask, green_mask, CV_8U);
    dilate(blue_mask, blue_mask, CV_8U);
    dilate(yellow_mask, yellow_mask, CV_8U);

    bitwise_and(img, img, res_red, red_mask);
    bitwise_and(img, img, res_green, green_mask);
    bitwise_and(img, img, res_blue, blue_mask);
    bitwise_and(img, img, res_yellow, yellow_mask);


    vector<Point> redCounter = detect_max_color(red_mask);
    vector<Point> greenCounter = detect_max_color(green_mask);
    vector<Point> blueCounter = detect_max_color(blue_mask);
    vector<Point> yellowCounter = detect_max_color(yellow_mask);
    show(img, redCounter, greenCounter, blueCounter, yellowCounter);


    namedWindow(argv[1], WINDOW_NORMAL);
    imshow(argv[1], img);
    waitKey(6000);
    destroyAllWindows();
}