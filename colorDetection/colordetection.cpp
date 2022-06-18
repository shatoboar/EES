//
// Created by tristan on 05.06.22.
//
#include <colordetection.h>
#include <../Controller/controller.h>
#include <iostream>
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

//int colour represents the colour of objects that are searched for, o=red, 1=green, 2=blue, 3=yellow
vector<Point> detect_max_color(Mat mask, int colour, ColorDetection *c) {
    int max_area = 0;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<cv::Point> large_contour;
    findContours( mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    int object_counter = 0;

    for(int i = 0; i < contours.size(); i++){
        int area = (int)cv::contourArea(contours[i]);
        if (area > 20000) {
            object_counter++;
            if(area> max_area)
            large_contour = contours[i];
            max_area = area;
        }
    }

    if(object_counter > 0){
        if(colour == 0){
            c->redBrick = true;
            if(object_counter > 1){
                c->redBrickOnlyOne = false;
            }else{
                c->redBrickOnlyOne = true;
            }
        }else if(colour == 1){
            c->greenBrick = true;
            if(object_counter > 1){
                c->greenBrickOnlyOne = false;
            }else{
                c->greenBrickOnlyOne = true;
            }
        }else if(colour == 2){
            c->blueBrick = true;
            if(object_counter > 1){
                c->blueBrickOnlyOne = false;
            }else{
                c->blueBrickOnlyOne = true;
            }
        }else if(colour == 3){
            c->yellowBrick = true;
            if(object_counter > 1){
                c->yellowBrickOnlyOne = false;
            }else{
                c->yellowBrickOnlyOne = true;
            }
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

bool ColorDetection::onlyOneColor() {
    if(redBrick != greenBrick != blueBrick != yellowBrick){
        return true;
    }

    return false;
}

Color ColorDetection::returnColor() {
    if (redBrick) {
        return Color(red);
    } else if (blueBrick) {
        return Color(blue);
    } else if (greenBrick) {
        return Color(green);
    }

    return Color(yellow);
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

    redCounter = detect_max_color(red_mask, 0,this);
    greenCounter = detect_max_color(green_mask, 1, this);
    blueCounter = detect_max_color(blue_mask, 2, this);
    yellowCounter = detect_max_color(yellow_mask, 3, this);
}

int main(int argc, char* argv[]){
    ColorDetection newDetection(argv[1]);
    if(newDetection.redBrick != newDetection.greenBrick != newDetection.blueBrick != newDetection.yellowBrick){
        if(newDetection.redBrick){
            if(newDetection.redBrickOnlyOne){
                cout << "Red Brick On Line";
            }else{
                cout << "More than one red brick on line";
            }
        }else if(newDetection.greenBrick){
            if(newDetection.greenBrickOnlyOne){
                cout << "Green Brick On Line";
            }else{
                cout << "More than one green brick on line";
            }
        }else if(newDetection.blueBrick){
            if(newDetection.blueBrickOnlyOne){
                cout << "Blue Brick On Line";
            }else{
                cout << "More than one blue brick on line ";
            }
        }else if(newDetection.yellowBrick){
            if(newDetection.yellowBrickOnlyOne){
                cout << "Yellow Brick On Line";
            }else{
                cout << "More than one yellow brick on line";
            }
        }
    }else if(!newDetection.redBrick && !newDetection.greenBrick && !newDetection.blueBrick && !newDetection.yellowBrick){
        cout << "Mistake no object on the line";
    }else{
        cout << "Mistake more than one colour of bricks on the line";
    }
    cout << "\n";
    show(newDetection.img, newDetection.redCounter, newDetection.greenCounter, newDetection.blueCounter, newDetection.yellowCounter, newDetection);
}