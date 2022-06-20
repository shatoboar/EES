#include <colordetection.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<Point> ColorDetection::get_red_counter() {
    return red_counter;
}

vector<Point> ColorDetection::get_green_counter() {
    return green_counter;
}

vector<Point> ColorDetection::get_blue_counter() {
    return blue_counter;
}

vector<Point> ColorDetection::get_yellow_counter() {
    return yellow_counter;
}

//int colour represents the colour of objects that are searched for, o=red, 1=green, 2=blue, 3=yellow
vector<Point> detect_max_color(Mat mask, Color search_color, ColorDetection *c) {
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
        switch(search_color){
            case Color::red:
                c->red_brick = true;
                if(object_counter > 1){
                    c->red_brick_only_one = false;
                }else{
                    c->red_brick_only_one = true;
                }
                break;
            case Color::green:
                c->green_brick = true;
                if(object_counter > 1){
                    c->green_brick_only_one = false;
                }else{
                    c->green_brick_only_one = true;
                }
                break;
            case Color::blue:
                c->blue_brick = true;
                if(object_counter > 1){
                    c->blue_brick_only_one = false;
                }else{
                    c->blue_brick_only_one = true;
                }
                break;
            case Color::yellow:
                c->yellow_brick = true;
                if(object_counter > 1){
                    c->yellow_brick_only_one = false;
                }else{
                    c->yellow_brick_only_one = true;
                }
                break;
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

void show(Mat img, vector<Point> red, vector<Point> green, vector<Point> blue, vector<Point> yellow, ColorDetection new_detection) {
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
    imshow("teST", img);
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

    Color red_c = Color::red;
    Color green_c = Color::green;
    Color blue_c = Color::blue;
    Color yellow_c = Color::yellow;

    red_counter = detect_max_color(red_mask, red_c,this);
    green_counter = detect_max_color(green_mask, green_c, this);
    blue_counter = detect_max_color(blue_mask, blue_c, this);
    yellow_counter = detect_max_color(yellow_mask, yellow_c, this);

    if(red_brick != green_brick != blue_brick != yellow_brick){
        if(red_brick){
            color_detection_result = Color_detected::red;
        }else if(green_brick){
            color_detection_result = Color_detected::green;
        }else if(blue_brick){
            color_detection_result = Color_detected::blue;
        }else if(yellow_brick){
            color_detection_result = Color_detected::yellow;
        }
    }else if(!red_brick && !green_brick && !blue_brick && !yellow_brick){
        color_detection_result = Color_detected::no_object;
    }else{
        color_detection_result = Color_detected::several_colors;
    }
}

int main(int argc, char* argv[]){
    ColorDetection new_detection(argv[1]);
    switch(new_detection.color_detection_result){
        case Color_detected::red:
            cout << "Red Brick on Line\n";
            break;
        case Color_detected::green:
            cout << "Green Brick on Line\n";
            break;
        case Color_detected::blue:
            cout << "Blue Brick on Line\n";
            break;
        case Color_detected::yellow:
            cout << "Yellow Brick on Line\n";
            break;
        case Color_detected::no_object:
            cout << "No Brick on Line\n";
            break;
        case Color_detected::several_colors:
            cout << "Several Colors on Line\n";
            break;
    }

    show(new_detection.img, new_detection.red_counter, new_detection.green_counter, new_detection.blue_counter, new_detection.yellow_counter, new_detection);
}