#include <stdio.h>

#include "API.h"

void log(char *text)
{
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

int main(int argc, char *argv[])
{
    log("Running...");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "start");
    // Right hand rule
    while (1)
    {
        // If there is no wall on the right, turn right
        if (!API_wallRight())
            API_turnRight();
        // while there is a wall in front, keep turning left
        while (API_wallFront())
            API_turnLeft();
        // move forward when an opening is discovered
        API_moveForward();
    }
}
