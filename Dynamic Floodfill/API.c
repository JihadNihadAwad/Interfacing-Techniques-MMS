#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 32

int cell_count = 0;
int turn_count = 0;
int prev_cell_count = 0;
int prev_turn_count = 0;

int getInteger(char* command) {
    printf("%s\n", command);
    fflush(stdout);
    char response[BUFFER_SIZE];
    fgets(response, BUFFER_SIZE, stdin);
    int value = atoi(response);
    return value;
}

int getBoolean(char* command) {
    printf("%s\n", command);
    fflush(stdout);
    char response[BUFFER_SIZE];
    fgets(response, BUFFER_SIZE, stdin);
    int value = (strcmp(response, "true\n") == 0);
    return value;
}

int getAck(char* command) {
    printf("%s\n", command);
    fflush(stdout);
    char response[BUFFER_SIZE];
    fgets(response, BUFFER_SIZE, stdin);
    int success = (strcmp(response, "ack\n") == 0);
    return success;
}

int API_mazeWidth() {
    return getInteger("mazeWidth");
}

int API_mazeHeight() {
    return getInteger("mazeHeight");
}

int API_wallFront() {
    return getBoolean("wallFront");
}

int API_wallRight() {
    return getBoolean("wallRight");
}

int API_wallLeft() {
    return getBoolean("wallLeft");
}

int API_moveForward() {
    cell_count++;
    return getAck("moveForward");
}

void API_turnRight() {
    turn_count++;
    getAck("turnRight");
}

void API_turnLeft() {
    turn_count++;
    getAck("turnLeft");
}

void API_setWall(int x, int y, char direction) {
    printf("setWall %d %d %c\n", x, y, direction);
    fflush(stdout);
}

void API_clearWall(int x, int y, char direction) {
    printf("clearWall %d %d %c\n", x, y, direction);
    fflush(stdout);
}

void API_setColor(int x, int y, char color) {
    printf("setColor %d %d %c\n", x, y, color);
    fflush(stdout);
}

void API_clearColor(int x, int y) {
    printf("clearColor %d %d\n", x, y);
    fflush(stdout);
}

void API_clearAllColor() {
    printf("clearAllColor\n");
    fflush(stdout);
}

void API_setText(int x, int y, char* text) {
    printf("setText %d %d %s\n", x, y, text);
    fflush(stdout);
}

void API_clearText(int x, int y) {
    printf("clearText %d %d\n", x, y);
    fflush(stdout);
}

void API_clearAllText() {
    printf("clearAllText\n");
    fflush(stdout);
}

int API_wasReset() {
    return getBoolean("wasReset");
}

int API_ackReset() {
    fprintf(stderr, "###########################\n");
    fprintf(stderr, "Cell Count: %d\nTurn Count: %d\n", cell_count, turn_count);
    fprintf(stderr, "###########################\n\n");
    if(prev_cell_count == cell_count && prev_turn_count == turn_count){
        fprintf(stderr,"Paths have converged. \nExitting...\n");
        return 0;
    }
    prev_cell_count = cell_count;
    prev_turn_count = turn_count;
    cell_count = 0;
    turn_count = 0;
    getAck("ackReset");
    return 1;
}
