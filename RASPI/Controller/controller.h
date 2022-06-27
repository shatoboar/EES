//
// Created by Timo Oeltze on 13.06.22.
//

#ifndef EES_CONTROLLER_H
#define EES_CONTROLLER_H
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


#endif //EES_CONTROLLER_H
