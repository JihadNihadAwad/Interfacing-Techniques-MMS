#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "API.h"

#define MAX 16 // Maximum maze size

// Direction vectors for movement (N, E, S, W)
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define BLACK 0    // Black
#define BLUE 1     // Blue
#define YELLOW 2     // Gray
#define CYAN 3     // Cyan
#define GREEN 4    // Green
#define ORANGE 5   // Orange
#define RED 6      // Red
#define WHITE 7    // White
#define GRAY 8   // Yellow
#define DARK_BLUE 9     // Dark Blue
#define DARK_CYAN 10    // Dark Cyan
#define DARK_GRAY 11    // Dark Gray
#define DARK_GREEN 12   // Dark Green
#define DARK_RED 13     // Dark Red
#define DARK_YELLOW 14  // Dark Yellow

// Track current position and direction
int currentRow = 0;
int currentColumn = 0;
int currentDirection = NORTH; // Assume initial direction is North

/*
The direction vectors dRow[] and dCol[]
are used to simplify movement within
a 2D grid (like a maze) by encoding changes
in row and column indices for each direction.
*/
int dRow[] = {1, 0, -1, 0};
int dCol[] = {0, 1, 0, -1};
char directions[4] = {'n', 'e', 's', 'w'};
char colors[] = {
    'k',	//Black
    'b',	//Blue
    'y',	//Yellow
    'c',	//Cyan
    'g',	//Green
    'o',	//Orange
    'r',	//Red
    'w',	//White
    'a',	//Gray
    'B',	//Dark Blue
    'C',	//Dark Cyan
    'A',	//Dark Gray
    'G',	//Dark Green
    'R',	//Dark Red
    'Y',	//Dark Yellow
};

char colour_names[][12] = {
    "Black",	    //Black
    "Blue",	        //Blue
    "Yellow",	    //Yellow
    "Cyan",	        //Cyan
    "Green",	    //Green
    "Orange",       //Orange
    "Red",	        //Red
    "White",	    //White
    "Gray",	        //Gray
    "Dark Blue",	//Dark Blue
    "Dark Cyan",	//Dark Cyan
    "Dark Gray",	//Dark Gray
    "Dark Green",	//Dark Green
    "Dark Red", 	//Dark Red
    "Dark Yellow",	//Dark Yellow
};

void log(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

// Structure to represent a cell in the maze
struct Cell {
    int row;
    int col;
    //manhattan distance
};
typedef struct Cell* cell;

cell create_cell(int row, int col){
    cell new_cell = malloc(sizeof(struct Cell));
    if (new_cell == NULL){
        exit(1);
    }
    new_cell->row = row;
    new_cell->col = col;
    return new_cell;
}

// Queue implementation for BFS
struct Queue {
    cell queue[MAX * MAX * 2];  //An array of cells
    int front, rear;
} ;
typedef struct Queue* queue;

void initQueue(queue q) {
    q->front = q->rear = 0;
}

queue create_queue(queue q){
    q = malloc(sizeof(struct Queue));
    if (q == NULL) {
        log("Memory allocation for queue failed!");
        exit(1);  // Handle memory allocation failure
    }
    initQueue(q);
    return q;
}

bool isQueueEmpty(queue q) {
    return q->front == q->rear;
}

void enqueue(queue q, cell c) {
    q->queue[q->rear++] = c;
}

cell dequeue(queue q) {
    return q->queue[q->front++];
}
void free_queue(queue q){
    if(q == NULL) return;

    while(!isQueueEmpty(q)){
        free(dequeue(q));
    }
    free(q);
}

// Function to check if a cell is within maze bounds
bool isValid(int row, int col) {
    return row >= 0 && row < MAX && col >= 0 && col < MAX;
}

// Function to display the maze distances
void printMaze(int maze[MAX][MAX]) {
    log("----------------------");
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if (maze[i][j] == -1)
                fprintf(stderr,"INF ");
            else
                fprintf(stderr, "%3d ", maze[i][j]);
        }
        log("");
    }
    log("----------------------");

}

// Function to display the maze's walls 
void printWalls(int walls[MAX][MAX][4]) {
    log("----------------------");
    int num_surrounding_walls = 0;
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            num_surrounding_walls = 0;
            for (int dir = 0; dir < 4; dir++) {
                if (walls[i][j][dir] == 1)
                    num_surrounding_walls += 1;
            }
        fprintf(stderr,"%3d", num_surrounding_walls);
        }
        log("");

    }
    log("----------------------");

}

// Function to perform flood-fill algorithm
void floodFill(int maze[MAX][MAX], int walls[MAX][MAX][4], cell goal_cells[4], int num_goal_cells) {
    
    //To keep track of the cells that are processed (so that distances are not recalculated which will make problems)
    int processed[MAX][MAX];
    char num[2];
    //Manhattan distance is used
    // Initialize distance grid
    int distance[MAX][MAX];
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            distance[i][j] = 1;
            //API_setColor(j, i, 'B');
            sprintf(num, "%d", distance[i][j]);
            API_setText(j, i, num);
            processed[i][j] = 0;
        }
    }
    

    API_setColor(0, 0, 'G');
    API_setText(0, 0, "start");
    //Set the 2x2 goal cells distance to 0 and stop them from being enqueued again (processed = 1)
    for(int i = 0; i < num_goal_cells; i++){
        distance[goal_cells[i]->row][goal_cells[i]->col] = 0;
        processed[goal_cells[i]->row][goal_cells[i]->col] = 1;
        API_setColor(goal_cells[i]->col, goal_cells[i]->row, 'G');
        sprintf(num, "%d", 0);
        API_setText(goal_cells[i]->col, goal_cells[i]->row, num);
    }
    
    
    
    
    cell added_cell;
    
    // BFS using a queue
    queue q = create_queue(q);
    
    
    //Enqueue the 2x2 goal cells
    for(int i = 0; i < num_goal_cells; i++)
        enqueue(q, goal_cells[i]);
    

    int count = 0;
    while (!isQueueEmpty(q)) {
        cell current_cell = dequeue(q);
        int cell_distance = distance[current_cell->row][current_cell->col];
        count += 1;
        // Explore all four directions (N, E, S, W)
        for (int dir = 0; dir < 4; dir++) {
            int newRow = current_cell->row + dRow[dir];
            int newCol = current_cell->col + dCol[dir];
            
            /* if(walls[current_cell->row][current_cell->col][dir] == 1)
                API_setWall(current_cell->col, current_cell->row, directions[dir]); */

            // Check if movement is valid and there's no wall in the current direction
            //Also check if the neighboring cell has ever entered the queue
            if (isValid(newRow, newCol) && walls[current_cell->row][current_cell->col][dir] == 0
            && processed[newRow][newCol] == 0) {

                processed[newRow][newCol] = 1; // prevent it from being enqueued by multiple cells near it
                // Update distance and enqueue the new cell
                distance[newRow][newCol] = cell_distance + 1;
                //API_setColor(newCol, newRow, 'B');
                sprintf(num, "%d", distance[newRow][newCol]);
                API_setText(newCol, newRow, num);
                added_cell = create_cell(newRow, newCol);
                enqueue(q, added_cell);
                
            }
            API_setColor(0, 0, 'G');
            API_setText(0, 0, "start");
        }
        if (count > 4)
            free(current_cell);
    }
    free(q);

    // Update the maze with flood-fill distances
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            maze[i][j] = distance[i][j];
        }
    }
    
}


// Function to initialize maze walls
void initializeMazeWalls(int walls[MAX][MAX][4]) {
    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            //directions (0,1,2,3) -> (N,E,S,W)
            for (int dir = 0; dir < 4; dir++) {
                // North boundary
                if (row == MAX - 1 && dir == 0) {
                    walls[row][col][dir] = 1;
                    API_setWall(col, row,  directions[dir]);
                }
                // East boundary
                else if (col == MAX - 1 && dir == 1) {
                    walls[row][col][dir] = 1;
                    API_setWall(col, row,  directions[dir]);

                }
                // South boundary
                else if (row == 0 && dir == 2) {
                    walls[row][col][dir] = 1;
                    API_setWall(col, row,  directions[dir]);

                }
                // West boundary
                else if (col == 0 && dir == 3) {
                    walls[row][col][dir] = 1;
                    API_setWall(col, row,  directions[dir]);

                }
                // Non-boundary walls (empty)
                else {
                    walls[row][col][dir] = 0;

                }
            }
        }
    }

}
void logWalls(int walls[MAX][MAX][4]) {
    // Check front
    if (API_wallFront()) {
        walls[currentRow][currentColumn][currentDirection] = 1;
        API_setWall(currentColumn, currentRow,  directions[currentDirection]);

        switch (currentDirection) {
            case NORTH:
                if (currentRow < MAX - 1){
                    walls[currentRow + 1][currentColumn][SOUTH] = 1;
                    API_setWall(currentColumn, currentRow + 1,  directions[SOUTH]);
                }
                break;
            case EAST:
                if (currentColumn < MAX - 1){
                    walls[currentRow][currentColumn + 1][WEST] = 1;
                    API_setWall(currentColumn + 1, currentRow,  directions[WEST]);
                }

                break;
            case SOUTH:
                if (currentRow > 0){
                    walls[currentRow - 1][currentColumn][NORTH] = 1;
                    API_setWall(currentColumn, currentRow - 1,  directions[NORTH]);
                }
                break;
            case WEST:
                if (currentColumn > 0){
                    walls[currentRow][currentColumn - 1][EAST] = 1;
                    API_setWall(currentColumn - 1, currentRow,  directions[EAST]);
                }
                break;
        }
    } 

    // Check right
    if (API_wallRight()) {
        int rightDir = (currentDirection + 1) % 4;
        walls[currentRow][currentColumn][rightDir] = 1;
        API_setWall(currentColumn, currentRow,  directions[rightDir]);

        switch (rightDir) {
            case NORTH:
                if (currentRow < MAX - 1){ 
                    walls[currentRow + 1][currentColumn][SOUTH] = 1;
                    API_setWall(currentColumn, currentRow + 1,  directions[SOUTH]);
                }
                break;
            case EAST:
                if (currentColumn < MAX - 1) {
                    walls[currentRow][currentColumn + 1][WEST] = 1;
                    API_setWall(currentColumn + 1, currentRow,  directions[WEST]);
                }
                break;
            case SOUTH:
                if (currentRow > 0) {
                    walls[currentRow - 1][currentColumn][NORTH] = 1;
                    API_setWall(currentColumn, currentRow - 1,  directions[NORTH]);
                }
                break;
            case WEST:
                if (currentColumn > 0) {
                    walls[currentRow][currentColumn - 1][EAST] = 1;
                    API_setWall(currentColumn - 1, currentRow,  directions[EAST]);
                }
                break;
        }
    }

    // Check left
    if (API_wallLeft()) {
        int leftDir = (currentDirection + 3) % 4;
        walls[currentRow][currentColumn][leftDir] = 1;
        API_setWall(currentColumn, currentRow,  directions[leftDir]);

        switch (leftDir) {
            case NORTH:
                if (currentRow < MAX - 1) {
                    walls[currentRow + 1][currentColumn][SOUTH] = 1;
                    API_setWall(currentColumn, currentRow + 1,  directions[SOUTH]);

                }
                break;
            case EAST:
                if (currentColumn < MAX - 1) {
                    walls[currentRow][currentColumn + 1][WEST] = 1;
                    API_setWall(currentColumn + 1, currentRow,  directions[WEST]);

                }
                break;
            case SOUTH:
                if (currentRow > 0) {
                    walls[currentRow - 1][currentColumn][NORTH] = 1;
                    API_setWall(currentColumn, currentRow - 1,  directions[NORTH]);

                }
                break;
            case WEST:
                if (currentColumn > 0) {
                    walls[currentRow][currentColumn - 1][EAST] = 1;
                    API_setWall(currentColumn - 1, currentRow,  directions[EAST]);

                }
                break;
        }
    }
}   

// Function to move the robot forward and update current position
int moveRobot(int color) {
    if (API_moveForward()) {
        switch (currentDirection) {
            case NORTH:
                currentRow++;
                if (color != -1) API_setColor(currentColumn, currentRow, colors[color]);
                break;
            case EAST:
                currentColumn++;
                if (color != -1) API_setColor(currentColumn, currentRow, colors[color]);
                break;
            case SOUTH:
                currentRow--;
                if (color != -1) API_setColor(currentColumn, currentRow, colors[color]);
                break;
            case WEST:
                currentColumn--;
                if (color != -1) API_setColor(currentColumn, currentRow, colors[color]);
                break;
        }
        return 1; // Successfully moved
    } else {
        log("cant move due to wall");
        return 0; // Failed to move (wall in front)
    }
}


// Function to start the maze exploration
void startExploration(int maze[MAX][MAX], int walls[MAX][MAX][4], cell goal_cells[], int num_goal_cells, int color) {
    log("Running...");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "start");
    // Run the flood-fill algorithm
    floodFill(maze, walls, goal_cells, num_goal_cells);

    while (1) {
        logWalls(walls);

        // Check if reached any of the goal cells
        for(int i = 0; i < num_goal_cells; i++) {
            if(goal_cells[i]->row == currentRow && goal_cells[i]->col == currentColumn){
                return;          // Exit exploration after reaching the goal
            }
        }

        

        // Check for dead end
        int frontWall = walls[currentRow][currentColumn][currentDirection];
        int rightWall = walls[currentRow][currentColumn][(currentDirection + 1) % 4];
        int leftWall = walls[currentRow][currentColumn][(currentDirection + 3) % 4];

        if (frontWall && rightWall && leftWall)
        {
			log("dead end");
            // Dead end reached, run floodfill and turn around
            API_turnRight();
            API_turnRight();
            currentDirection = (currentDirection + 2) % 4;
			floodFill(maze, walls, goal_cells, num_goal_cells); // Recalculate floodfill
            continue;
        }

        // Find the best direction to move based on floodfill values
        int bestDirection = -1;
        int minDistance = 9999; // Initialize with a large value

        if(!frontWall){
            int nextRow = currentRow + dRow[currentDirection];
            int nextCol = currentColumn + dCol[currentDirection];
            if (isValid(nextRow, nextCol) && maze[nextRow][nextCol] < minDistance) {
                    minDistance = maze[nextRow][nextCol];
                    bestDirection = currentDirection;
                }
        }
        if(!rightWall){
            int nextRow = currentRow + dRow[(currentDirection + 1) % 4];
            int nextCol = currentColumn + dCol[(currentDirection + 1) % 4];
            if (isValid(nextRow, nextCol) && maze[nextRow][nextCol] < minDistance) {
                    minDistance = maze[nextRow][nextCol];
                    bestDirection = (currentDirection + 1) % 4;
                }
        }
        if(!leftWall){
            int nextRow = currentRow + dRow[(currentDirection + 3) % 4];
            int nextCol = currentColumn + dCol[(currentDirection + 3) % 4];
            if (isValid(nextRow, nextCol) && maze[nextRow][nextCol] < minDistance) {
                    minDistance = maze[nextRow][nextCol];
                    bestDirection = (currentDirection + 3) % 4;
                }
        }
		
		int actual_lowest_distance = 9999;
		for(int i=0;i < 4; i++){
			int nextRow = currentRow + dRow[i];
            int nextCol = currentColumn + dCol[i];
			if (isValid(nextRow, nextCol) && maze[nextRow][nextCol] < actual_lowest_distance) {
                    actual_lowest_distance = maze[nextRow][nextCol];
            }
		}

		if(actual_lowest_distance != minDistance){
			floodFill(maze, walls, goal_cells, num_goal_cells); // Recalculate floodfill
		}
        // Make a move based on the best direction
        if (bestDirection != -1) {
            // Turn towards the best direction
            while (currentDirection != bestDirection) {
                if ((currentDirection + 1) % 4 == bestDirection) {
                    API_turnRight();
                    currentDirection = (currentDirection + 1) % 4;
                } else {
                    API_turnLeft();
                    currentDirection = (currentDirection + 3) % 4;
                }
            }
            // Move forward
            moveRobot(color);
        } 
            
        
    }
}
    
int main() {
    char num[2];
    //2x2 goal cells
    cell goal_cells[4]; // Example goal position
    for(int i = 0; i < 4; i++)
        goal_cells[i] = create_cell(MAX/2 - 1,MAX/2 - 1);

    goal_cells[1]->col = goal_cells[0]->col + 1;

    goal_cells[2]->row = goal_cells[0]->row + 1;

    goal_cells[3]->row = goal_cells[0]->row + 1;
    goal_cells[3]->col = goal_cells[0]->col + 1;

    

    //a walls matrix (all the cells with each cell having four directions N,E,S,W)
    int walls[MAX][MAX][4];
    initializeMazeWalls(walls);
    // array that contains the final distance results
    int maze[MAX][MAX] = {0};

    for(int i = 1; i < 12; i++){
        fprintf(stderr, "Iteration: %d, Colour: %s\n", i, colour_names[i]);
        startExploration(maze, walls, goal_cells, 4, i );
        if(!API_ackReset()) // when it converges, auto exits
            break;
        currentRow = 0;
        currentColumn = 0;
        currentDirection = NORTH;
    }



    for (int i = 0; i < 4; i++){
        free(goal_cells[i]);
    }
    

    return 0;
}
