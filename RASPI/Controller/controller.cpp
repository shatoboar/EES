#include "controller.h"
#include "../colorDetection/colordetection.h"

int init_ps() {
    return 4;
}

bool hasConnection(int value) {
    if (value > 0) {
        return true;
    }

    return false;
}

bool readyForNewObject() {
    //TODO: readyForNewObject from API
    return true;
}

/*void mainRoutine(Controller newController) {
    bool ready = readyForNewObject();

    while (ready) {
        //TODO: take picture
        char* filepath = "../picturesFinalPosition/blueBrickOnLineLeft.jpg";
        ColorDetection newColorDetection(filepath);

        if (newColorDetection.onlyOneColor()) {

        } else if (//){
            //TODO: return error (255) - more then one brick (same color) on the line, no brick on the line
        }

        ready = readyForNewObject();
    }

}*/

int main(int argc, char* argv[]){
    /*Controller newController;
    newController.buckets = init_ps();
    newController.hasConnection = hasConnection(newController.buckets);

    if (newController.hasConnection) {
        //mainRoutine(newController);
    }*/
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
            cout << "No detected colour on Line\n";
            break;
        case Color_detected::several_colors:
            cout << "Several Colors on Line\n";
            break;
    }
    switch (new_detection.size_detection_result) {
        case Size_detected::two_times_four:
            cout << "2x4 detected\n";
            break;
        case Size_detected::two_times_three:
            cout << "2x3 detected\n";
            break;
        case Size_detected::two_times_two:
            cout << "2x2 detected\n";
            break;
        case Size_detected::serveral_sizes:
            cout << "More than 1 size detected\n";
            break;
        case Size_detected::no_bricks:
            cout << "No bricks on line\n";
            break;
    }
}

Controller::Controller() {
    hasConnection = false;
}
