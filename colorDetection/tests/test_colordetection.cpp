//
// Created by Timo Oeltze on 15.06.22.
//

#include "test_colordetection.h"
#include "colordetection.h"
#include <gtest/gtest.h>

TEST(BlueBrickDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/blueBrickOnLineLeft.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::blue);
}

TEST(TwoBlueBricksDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/twoBlueOnLine.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::blue);
}

TEST(RedBrickDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/redBrickOnLineLeft.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::red);
}

TEST(TwoRedBricksDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/twoRedOnLine.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::red);
}

TEST(GreenBrickDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/greenBrickOnLineLeft.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::green);
}

TEST(TwoGreenBricksDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/twoGreenOnLine.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::green);
}

TEST(YellowBrickDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/yellowBrickOnLineLeft.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::yellow);
}

TEST(TwoYellowBricksDetection, Detected) {
    char filepath[] = "./picturesFinalPosition/twoYellowOnLine.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::yellow);
}

TEST(EmptyLine, Detected) {
    char filepath[] = "./picturesFinalPosition/NothingOnLine.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::no_object);
}

TEST(TwoDifferentBricks, Detected) {
    char filepath[] = "./picturesFinalPosition/twobricksOnLine.jpg";
    ColorDetection newDetection(filepath);
    EXPECT_EQ(newDetection.color_detection_result, Color_detected::several_colors);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}