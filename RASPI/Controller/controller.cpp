#include "controller.h"
#include "../colorDetection/colordetection.h"
#include "../BluetoothService/BluetoothService.h"

/**
 * take a picture and save result in class var detected
 */
void Controller::analysePicture() {
    system("raspistill -o pic.jpg -t 100");
    ColorDetection analyzer("pic.jpg");

    system("rm pic.jpg");
    detected = pair<Color_detected, Size_detected>(analyzer.color_detection_result, analyzer.size_detection_result);
}

/**
 * @return bucket number in which a stone should be sorted
 */
int Controller::bucketSortColor() {

    for (int index = 0; index < sortedBuckets.size(); index++) {
        pair<Color_detected, Size_detected> element = sortedBuckets.at(index);

        if (element.first == detected.first) {
            return index+2;
        }

    }

    if (usedBuckets < numberBuckets) {
        if (detected.first != Color_detected::no_object && detected.first != Color_detected::several_colors) {
            sortedBuckets.push_back(detected);
            usedBuckets += 1;
            return usedBuckets;
        }
    }

    return 1;
}

int Controller::bucketSortSize() {
    for (int index = 0; index < sortedBuckets.size(); index++) {
        pair<Color_detected, Size_detected> element = sortedBuckets.at(index);

        if (element.second == detected.second) {
            return index+2;
        }

    }

    if (usedBuckets < numberBuckets) {
        if (detected.second != Size_detected::no_bricks && detected.second != Size_detected::serveral_sizes
        && detected.second != Size_detected::unknown_sizes) {
            sortedBuckets.push_back(detected);
            usedBuckets += 1;
            return usedBuckets;
        }
    }

    return 1;
}

int Controller::bucketSortColorSize() {

}

void mainRoutine(Controller controller, BluetoothService bl_service) {
    controller.hasConnection = true;

    while (controller.hasConnection) {
        int bucketID;

        bl_service.DeployRoutine();

        controller.analysePicture();

        switch (controller.mode) {
            case SortingMode::colorOnly:
                bucketID = controller.bucketSortColor();
                break;
            case SortingMode::shapeOnly:
                bucketID = controller.bucketSortSize();
                break;
            case SortingMode::colorAndShape:
                bucketID = controller.bucketSortColorSize();
                break;
        }

        bl_service.SendClassificationRoutine(bucketID);
    }

    bl_service.CloseConnection();
}

int main(int argc, char* argv[]){
    Controller controller(SortingMode::colorOnly);
    BluetoothService bl_service;

    controller.numberBuckets = bl_service.InitRoutine();
    controller.usedBuckets = 1;
    controller.sortedBuckets = {};
    mainRoutine(controller, bl_service);
}

Controller::Controller(SortingMode setMode) {
    mode = setMode;
}
