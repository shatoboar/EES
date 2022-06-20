#include "test_colordetection.h"
#include "colordetection.h"

bool colorTest(char* filePath, Color_detected expectation) {
    ColorDetection newDetection(filePath);

    if (newDetection.color_detection_result != expectation) {
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    int result = 0;
    list<pair<char*, Color_detected>> testInput = {
            pair<char*, Color_detected>("../picturesFinalPosition/blueBrickOnLineLeft.jpg", Color_detected::blue),
            pair<char*, Color_detected>("../picturesFinalPosition/twoBlueOnLine.jpg", Color_detected::blue),
            pair<char*, Color_detected>("../picturesFinalPosition/redBrickOnLineLeft.jpg", Color_detected::red),
            pair<char*, Color_detected>("../picturesFinalPosition/twoRedOnLine.jpg", Color_detected::red),
            pair<char*, Color_detected>("../picturesFinalPosition/greenBrickOnLineLeft.jpg", Color_detected::green),
            pair<char*, Color_detected>("../picturesFinalPosition/twoGreenOnLine.jpg", Color_detected::green),
            pair<char*, Color_detected>("../picturesFinalPosition/yellowBrickOnLineLeft.jpg", Color_detected::yellow),
            pair<char*, Color_detected>("../picturesFinalPosition/twoYellowOnLine.jpg", Color_detected::yellow),
            pair<char*, Color_detected>("../picturesFinalPosition/NothingOnLine.jpg", Color_detected::no_object),
            pair<char*, Color_detected>("../picturesFinalPosition/twobricksOnLine.jpg", Color_detected::several_colors)
    };

    for (pair<char*, Color_detected> test : testInput) {
        if(!colorTest(test.first, test.second)) {
            cout << "Test for file: " << test.first << " failed /n";
            result++;
        }
    }

    cout << "\n -------- Number of tests: " << testInput.size() << ", tests failed: " << result << " -------- \n";

    return result;
}