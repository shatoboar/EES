//
// Created by Timo Oeltze on 15.06.22.
//

#include "test_colordetection.h"
#include "colordetection.h"

int main(int argc, char* argv[]) {
    char filePath[] = "./picturesFinalPosition/redBrickOnLineMiddle.jpg";
    ColorDetection newColorDetection(filePath);
    Color_detected result = newColorDetection.color_detection_result;

    if (Color_detected::blue == result) {
        return 0;
    } else {
        return 1;
    }
}
