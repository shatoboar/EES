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
    Controller newController;
    newController.buckets = init_ps();
    newController.hasConnection = hasConnection(newController.buckets);

    if (newController.hasConnection) {
        //mainRoutine(newController);
    }
}

Controller::Controller() {
    hasConnection = false;
}
