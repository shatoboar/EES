#include "test_colordetection.h"
#include "colordetection.h"

bool colorTest(char* filePath, Color_detected expectation) {
    ColorDetection newDetection(filePath);

    if (newDetection.color_detection_result != expectation) {
        return false;
    }

    return true;
}

bool sizeTest(char* filePath, Size_detected expectation) {
    ColorDetection newDetection(filePath);

    if (newDetection.size_detection_result != expectation) {
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    int result = 0;
    list<pair<char*, Color_detected>> testInputColor = {
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/blueBrickOnLineLeft.jpg", Color_detected::blue),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/twoBlueOnLine.jpg", Color_detected::blue),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/redBrickOnLineLeft.jpg", Color_detected::red),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/twoRedOnLine.jpg", Color_detected::red),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/greenBrickOnLineLeft.jpg", Color_detected::green),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/twoGreenOnLine.jpg", Color_detected::green),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/yellowBrickOnLineLeft.jpg", Color_detected::yellow),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/twoYellowOnLine.jpg", Color_detected::yellow),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/NothingOnLine.jpg", Color_detected::no_object),
            pair<char*, Color_detected>("../RASPI/colorDetection/tests/picturesFinalPosition/twobricksOnLine.jpg", Color_detected::several_colors)
    };

    list<pair<char*, Size_detected>> testInputSize = {
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/bricktoobig_yellow.jpg", Size_detected::unknown_sizes),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/emptyline.jpg", Size_detected::no_bricks),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_four_red_hochkant.jpg", Size_detected::two_times_four),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_four_red_middle.jpg", Size_detected::two_times_four),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_four_red_middle_upsidedown.jpg", Size_detected::two_times_four),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_four_red_schraeg.jpg", Size_detected::two_times_four),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_four_red_two_two_red.jpg", Size_detected::serveral_sizes),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_three_red_middle.jpg", Size_detected::two_times_three),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_two_red_middle.jpg", Size_detected::two_times_two),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_two_yellow_two_four_red.jpg", Size_detected::serveral_sizes),
            pair<char*, Size_detected>("../RASPI/colorDetection/tests/pictureSizes/two_two_yellow_two_two_yellow.jpg", Size_detected::two_times_two)
    };

    for (pair<char*, Color_detected> test : testInputColor) {
        if(!colorTest(test.first, test.second)) {
            cout << "Test for file: " << test.first << " failed /n";
            result++;
        }
    }

    for (pair<char*, Size_detected> test : testInputSize) {
        if(!sizeTest(test.first, test.second)) {
            cout << "Test for file: " << test.first << " failed /n";
            result++;
        }
    }

    cout << "\n -------- Number of tests: " << testInputColor.size() + testInputSize.size() << ", tests failed: " << result << " -------- \n";

    return result;
}