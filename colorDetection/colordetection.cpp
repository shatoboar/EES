//
// Created by tristan on 05.06.22.
//
#include <colordetection.h>
#include <iostream>
#include <string.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point> ColorDetection::getRedCounter() {
    return redCounter;
}

vector<Point> ColorDetection::getGreenCounter() {
    return greenCounter;
}

vector<Point> ColorDetection::getBlueCounter() {
    return blueCounter;
}

vector<Point> ColorDetection::getYellowCounter() {
    return yellowCounter;
}

vector<Point> detect_max_color(Mat mask) {
    int max_area = 0;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<cv::Point> large_contour;
    findContours( mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

    for(int i = 0; i < contours.size(); i++){
        int area = (int)cv::contourArea(contours[i]);
        if (area > max_area && area > 4000) {
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

void show(Mat img, vector<Point> red, vector<Point> green, vector<Point> blue, vector<Point> yellow, ColorDetection newDetection) {
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
    namedWindow("TEST", WINDOW_NORMAL);
    imshow("teST", newDetection.img);
    waitKey(12000);
    destroyAllWindows();
}

ColorDetection::ColorDetection(char* filename) {
    Mat img_hsv;
    Mat red_mask, green_mask, blue_mask, yellow_mask;
    Mat res_red, res_green, res_blue, res_yellow;

    // read picture
    img = imread(filename);

    // convert from RGB to HSV
    cvtColor(img, img_hsv, COLOR_BGR2HSV);

    //define red mask
    inRange(img_hsv, red_lower, red_upper, red_mask);

    //define green mask
    inRange(img_hsv, green_lower, green_upper, green_mask);

    //define blue mask
    inRange(img_hsv, blue_lower, blue_upper, blue_mask);

    //define yellow mask
    inRange(img_hsv, yellow_lower,yellow_upper, yellow_mask);

    dilate(red_mask, red_mask, CV_8U);
    dilate(green_mask, green_mask, CV_8U);
    dilate(blue_mask, blue_mask, CV_8U);
    dilate(yellow_mask, yellow_mask, CV_8U);

    bitwise_and(img, img, res_red, red_mask);
    bitwise_and(img, img, res_green, green_mask);
    bitwise_and(img, img, res_blue, blue_mask);
    bitwise_and(img, img, res_yellow, yellow_mask);

    redCounter = detect_max_color(red_mask);
    greenCounter = detect_max_color(green_mask);
    blueCounter = detect_max_color(blue_mask);
    yellowCounter = detect_max_color(yellow_mask);

    if(redCounter.size() > 0){
        redBrick = true;
    }else{
        redBrick = false;
    }

    if(greenCounter.size() > 0){
        greenBrick = true;
    }else{
        greenBrick = false;
    }

    if(blueCounter.size() > 0){
        blueBrick = true;
    }else{
        blueBrick = false;
    }

    if(yellowCounter.size() > 0){
        yellowBrick = true;
    }else{
        yellowBrick = false;
    }
}

int main(int argc, char* argv[]){
    ColorDetection newDetection(argv[1]);
    if(newDetection.redBrick != newDetection.greenBrick != newDetection.blueBrick != newDetection.yellowBrick){
        if(newDetection.redBrick){
            cout << "Red Brick On Line";
        }else if(newDetection.greenBrick){
            cout << "Green Brick On Line";
        }else if(newDetection.blueBrick){
            cout << "Blue Brick On Line";
        }else if(newDetection.yellowBrick){
            cout << "Yellow Brick On Line";
        }
    }else{
        cout << "Mistake more than one brick";
    }
    cout << "\n";
    //show(newDetection.img, newDetection.redCounter, newDetection.greenCounter, newDetection.blueCounter, newDetection.yellowCounter, newDetection);
}