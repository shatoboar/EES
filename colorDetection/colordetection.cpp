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

vector<Point>detect_max_color(Mat mask) {
    double largestArea = 0;
    double largest_area = 0;
    int largest_contour_index = -1;
    vector<vector<Point> > contours; // is like contours, hierarchy in python
    findContours(mask.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    contours.resize(contours.size());

    for (size_t k = 0; k < contours.size(); k++) {
        double a = contourArea(contours[k], false);  //  Find the area of contour
        if (a > largest_area) {
            largest_area = a;
            largest_contour_index = k;                //Store the index of largest contour
        }
    }

    return contours[largest_area-1];
}

def box_creater(contour, r, g, b, name):
x, y, w, h = cv2.boundingRect(contour)
cv2.rectangle(img, (x, y),
(x + w, y + h),
(r, g, b), 2)

cv2.putText(img, name, (x, y),
cv2.FONT_HERSHEY_SIMPLEX,
2.0, (0, 0, 0))

int main( int argc, char *argv[]) {
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
    Scalar green_upper = Scalar(76, 25, 255);
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


}