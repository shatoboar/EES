
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../colorDetection/colordetection.h"
using namespace std;
using namespace cv;

enum SortingMode {colorOnly, shapeOnly, colorAndShape};

class Controller {
public:
    pair<Color_detected, Size_detected> detected;
    vector<pair<Color_detected, Size_detected>> sortedBuckets;
    int numberBuckets;
    int usedBuckets;
    SortingMode mode;
    bool hasConnection;
    Mat img;
    Controller(SortingMode setMode);
    void analysePicture();
    int bucketSortColor();
    int bucketSortSize();
    int bucketSortColorSize();
};

#endif