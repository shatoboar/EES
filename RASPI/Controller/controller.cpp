#include "controller.h"
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
 * @return bucket number in which a stone should be sorted by color
 */
int Controller::bucketSortColor() {

    //check whether the color is already mapped to a bucket
    for (int index = 0; index < sortedBuckets.size(); index++) {
        pair<Color_detected, Size_detected> element = sortedBuckets.at(index);

        //first pair is the color of a stone
        if (element.first == detected.first) {
            return index+2;
        }

    }

    //are buckets left to sort / map stones to
    if (usedBuckets < numberBuckets) {
        //conditions that no bucket should be mapped
        if (detected.first != Color_detected::no_object && detected.first != Color_detected::several_colors) {
            sortedBuckets.push_back(detected);
            usedBuckets += 1;
            return usedBuckets;
        }
    }

    //is rest bucket
    return 1;
}

/**
 * @return bucket number in which a stone should be sorted by size
 */
int Controller::bucketSortSize() {

    //check whether the size of a stone is already mapped to a bucket
    for (int index = 0; index < sortedBuckets.size(); index++) {
        pair<Color_detected, Size_detected> element = sortedBuckets.at(index);

        //second pair is the size of a stone
        if (element.second == detected.second) {
            return index+2;
        }

    }

    //are buckets left to sort / map stones to
    if (usedBuckets < numberBuckets) {
        //conditions that no bucket should be mapped
        if (detected.second != Size_detected::no_bricks && detected.second != Size_detected::serveral_sizes
            && detected.second != Size_detected::unknown_sizes) {
            sortedBuckets.push_back(detected);
            usedBuckets += 1;
            return usedBuckets;
        }
    }

    //is rest bucket
    return 1;
}

/**
 * @return bucket number in which a stone should be sorted by combination of color and size
 */
int Controller::bucketSortColorSize() {

    //check whether the combination of size and color of a stone is already mapped to a bucket
    for (int index = 0; index < sortedBuckets.size(); index++) {
        pair<Color_detected, Size_detected> element = sortedBuckets.at(index);

        if (element == detected) {
            return index+2;
        }
    }

    //are buckets left to sort / map stones to
    if (usedBuckets < numberBuckets) {
        //conditions that no bucket should be mapped (for color and size)
        if (detected.second != Size_detected::no_bricks && detected.second != Size_detected::serveral_sizes
            && detected.second != Size_detected::unknown_sizes && detected.first != Color_detected::several_colors) {
            sortedBuckets.push_back(detected);
            usedBuckets += 1;
            return usedBuckets;
        }
    }

    //is rest bucket
    return 1;
}

/**
 * loop for taking picture of stone on the line, analysing them and sending mapped bucket for stone to NXT
 * @param controller
 * @param bl_service
 */
void mainRoutine(Controller controller, BluetoothService bl_service) {
    //maybe later in use to close bluetooth connection
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