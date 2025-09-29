#include <stdio.h>
#include <stdlib.h>
#include "API.h"

void log(char *text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

int main(int argc, char *argv[]) {
    log("Running...");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "start");

    while (1) {
        // Randomly decide whether to try turning right first or going straight
        if (rand() % 2 == 0) { // 50% chance
            // Try turning right
            if (!API_wallRight()) {
                API_turnRight();
                API_moveForward();
            } else if (!API_wallFront()) {
                API_moveForward();
            } else {
                API_turnLeft();
            }

        } else {
            // Try going straight first
            if (!API_wallFront()) {
                API_moveForward();
            }
            // If there is a wall to the right turn left
            else if(API_wallRight()){
                 API_turnLeft();
            } else {
                 API_turnRight();
                 API_moveForward();
             }
         }
     }
}