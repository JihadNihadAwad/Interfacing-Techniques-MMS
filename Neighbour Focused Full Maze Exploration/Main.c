#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
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

// Maze data
int walls[MAX][MAX][4];
int maze[MAX][MAX] = {0};
bool visited[MAX][MAX] = {false};
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

// Function to perform flood-fill algorithm
void floodFill(cell goal_cells[], int num_goal_cells, int updateVisuals) {
    if(updateVisuals)
        API_setColor(0, 0, 'G');
        API_setText(0, 0, "start");
    //To keep track of the cells that are processed (so that distances are not recalculated which will make problems)
    int processed[MAX][MAX];
    char num[2];
    //Manhattan distance is used
    // Initialize distance grid
    int distance[MAX][MAX];
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            distance[i][j] = 1;
            if(updateVisuals){
                //API_setColor(j, i, 'B');
                sprintf(num, "%d", distance[i][j]);
                API_setText(j, i, num);
            }
            processed[i][j] = 0;
        }
    }
    
    //Set the 2x2 goal cells distance to 0 and stop them from being enqueued again (processed = 1)
    for(int i = 0; i < num_goal_cells; i++){
        distance[goal_cells[i]->row][goal_cells[i]->col] = 0;
        processed[goal_cells[i]->row][goal_cells[i]->col] = 1;
        if(updateVisuals){
            API_setColor(goal_cells[i]->col, goal_cells[i]->row, 'G');
            sprintf(num, "%d", 0);
            API_setText(goal_cells[i]->col, goal_cells[i]->row, num);
        }
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
                if(updateVisuals){
                    sprintf(num, "%d", distance[newRow][newCol]);
                    API_setText(newCol, newRow, num);
                }
                added_cell = create_cell(newRow, newCol);
                enqueue(q, added_cell);
                
            }
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
void initializeMazeWalls() {
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

void logWalls() {
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

int manhattan(int initRow, int initCol, int targetRow, int targetCol){
    return abs(initRow - targetRow) + abs(initCol - targetCol);
}

int heuristicDistance(int initRow, int initCol, int targetRow, int targetCol) {
    int penalty = 0;

    // Check LOS and add penalties for walls along the path
    bool losBlocked = false;

    // Row-wise wall checks
    if (initRow != targetRow) {
        int step = (targetRow > initRow) ? 1 : -1;
        int dir = (step > 0) ? 2 : 0; // 2 = South, 0 = North
        for (int r = initRow; r != targetRow; r += step) {
            if (walls[r][initCol][dir]) {
                penalty += 2; // Add detour penalty
                losBlocked = true;
            }
        }
    }

    // Column-wise wall checks
    if (initCol != targetCol) {
        int step = (targetCol > initCol) ? 1 : -1;
        int dir = (step > 0) ? 1 : 3; // 1 = East, 3 = West
        for (int c = initCol; c != targetCol; c += step) {
            if (walls[initRow][c][dir]) {
                penalty += 2; // Add detour penalty
                losBlocked = true;
            }
        }
    }

    // Add penalties for turns (if LOS is blocked)
    if (losBlocked) {
        int rowDir = (targetRow > initRow) ? 2 : (targetRow < initRow) ? 0 : -1; // South/North
        int colDir = (targetCol > initCol) ? 1 : (targetCol < initCol) ? 3 : -1; // East/West

        if (rowDir != -1 && colDir != -1) {
            // Corner or diagonal turn required
            penalty += 3; // Minimum penalty for turns
        } else if (rowDir != -1) {
            // Straight row movement
            penalty += (currentDirection + 2) % 4 == rowDir ? 3 : 0;
        } else if (colDir != -1) {
            // Straight column movement
            penalty += (currentDirection + 2) % 4 == colDir ? 3 : 0;
        }
    }

    // Corridor bonus (tight spaces, to avoid backtracking)
    for (int r = initRow; r <= targetRow; r++) {
        for (int c = initCol; c <= targetCol; c++) {
            int openCount = 0;
            if (!walls[r][c][0]) openCount++; // North
            if (!walls[r][c][1]) openCount++; // East
            if (!walls[r][c][2]) openCount++; // South
            if (!walls[r][c][3]) openCount++; // West

            if (openCount == 2)
                penalty -= 2;
            else if(openCount == 1)
                penalty -= 5;
        }
    }

    // Combine Manhattan distance with penalties
    return manhattan(initRow, initCol, targetRow, targetCol) + penalty;
}

int moveToAdjacentUnvisitedCell(cell goal_cells[], int num_goal_cells, int colour){
    int moved = 0;
    
    for(int i=0; i < 3;i++) {
        logWalls();
        visited[currentRow][currentColumn] = true;
        for(int i = 0; i < num_goal_cells; i++) {
            if(goal_cells[i]->row == currentRow && goal_cells[i]->col == currentColumn){
                return moved;          // Exit exploration after reaching the goal
            }
        }
        
        int frontWall = walls[currentRow][currentColumn][currentDirection];
        int rightWall = walls[currentRow][currentColumn][(currentDirection + 1) % 4];
        int leftWall = walls[currentRow][currentColumn][(currentDirection + 3) % 4];
        int direction = -1;
        int distance = 9999;

        int temp;
        int openCount = 0;
         if(!frontWall){
            int nextRow = currentRow + dRow[currentDirection];
            int nextCol = currentColumn + dCol[currentDirection];
            if (isValid(nextRow, nextCol) && !visited[nextRow][nextCol]) {
                openCount++;
                direction = currentDirection;
                distance = heuristicDistance(currentRow, currentColumn, nextRow, nextCol) - 2 * maze[nextRow][nextCol];
            }
        } 
        if(!rightWall){
            int nextRow = currentRow + dRow[(currentDirection + 1) % 4];
            int nextCol = currentColumn + dCol[(currentDirection + 1) % 4];
            if (isValid(nextRow, nextCol) && !visited[nextRow][nextCol]) {
                openCount++;
                temp =  heuristicDistance(currentRow, currentColumn, nextRow, nextCol) - 2 * maze[nextRow][nextCol];
                if(temp <= distance){
                    direction = (currentDirection + 1) % 4;
                    distance = temp;
                }
            }
        }
        if(!leftWall){
            int nextRow = currentRow + dRow[(currentDirection + 3) % 4];
            int nextCol = currentColumn + dCol[(currentDirection + 3) % 4];
            if (isValid(nextRow, nextCol) && !visited[nextRow][nextCol]) {
                openCount++;
                temp = heuristicDistance(currentRow, currentColumn, nextRow, nextCol) - 2 * maze[nextRow][nextCol];
                if(temp <= distance){
                    direction = (currentDirection + 3) % 4;
                    distance = temp;
                }
            }
        }
        if(direction == -1)
            break;
        if(openCount == 3)
            i--;
        while (currentDirection != direction) {
            if ((currentDirection + 1) % 4 == direction) {
                API_turnRight();
                currentDirection = (currentDirection + 1) % 4;
            } else {
                API_turnLeft();
                currentDirection = (currentDirection + 3) % 4;
            }
        }
        // Move forward
        moveRobot(colour);
        moved = 1;
        
    }
    return moved;
}

void startExploration(cell goal_cells[], int num_goal_cells, int color, int check_nearest) {
    // Run the flood-fill algorithm
    floodFill(goal_cells, num_goal_cells, 1);

    while(1) {
        if(check_nearest && moveToAdjacentUnvisitedCell(goal_cells, num_goal_cells, color)){
            return;
        }
        logWalls(walls);
        visited[currentRow][currentColumn] = true;
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
            // Dead end reached, run floodfill and turn around
            API_turnRight();
            API_turnRight();
            currentDirection = (currentDirection + 2) % 4;
            floodFill(goal_cells, num_goal_cells, 1); 
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
            floodFill(goal_cells, num_goal_cells, 1); // Recalculate floodfill
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

int getSixteenth(int row, int col, int sixteenthSizeRow, int sixteenthSizeCol) {
    int rowIndex = row / sixteenthSizeRow;
    int colIndex = col / sixteenthSizeCol;
    return rowIndex * 4 + colIndex; // Flattened 4x4 index
}

int calculateProximity(int section1, int section2) {
    // Define the center coordinates of each section
    int sectionCenters[16][2] = {
        {2, 2}, {2, 6}, {2, 10}, {2, 14},
        {6, 2}, {6, 6}, {6, 10}, {6, 14},
        {10, 2}, {10, 6}, {10, 10}, {10, 14},
        {14, 2}, {14, 6}, {14, 10}, {14, 14}
    };

    // Get the coordinates of the two sections
    int x1 = sectionCenters[section1][0];
    int y1 = sectionCenters[section1][1];
    int x2 = sectionCenters[section2][0];
    int y2 = sectionCenters[section2][1];

    // Calculate Manhattan distance between the two centers
    return abs(x1 - x2) + abs(y1 - y2);
}

void exploreUnvisitedCells() {
    cell unvisitedCells[MAX * MAX] = {NULL};
    
    cell currentCell[1] = {create_cell(currentRow, currentColumn)};

    int unvisitedCount = 0;

    // Collect all unvisited cells
    for (int row = 0; row < MAX; row++) {
        for (int col = 0; col < MAX; col++) {
            if (!visited[row][col]) {
                unvisitedCells[unvisitedCount] = create_cell(row, col);
                unvisitedCount++;
            }
        }
    }

    // Define sixteenths (4x4 grid)
    int sixteenthSizeRow = MAX / 4;
    int sixteenthSizeCol = MAX / 4;

    int sixteenthCounts[16] = {0}; // Track unvisited cells in each sixteenth
    int sixteenthWeights[16] = {0}; // Adjust exploration weights per sixteenth

    // Count unvisited cells in each sixteenth
    for (int i = 0; i < unvisitedCount; i++) {
        int row = unvisitedCells[i]->row;
        int col = unvisitedCells[i]->col;
        int sixteenth = getSixteenth(row, col, sixteenthSizeRow, sixteenthSizeCol);
        sixteenthCounts[sixteenth]++;
    }

    // Assign initial weights
    for (int s = 0; s < 16; s++) {
        sixteenthWeights[s] = unvisitedCount - sixteenthCounts[s];
    }


    int temp = unvisitedCount;
    int colour = DARK_RED;
    // Iterate until all unvisited cells are visited
    while (unvisitedCount > 0) {
        currentCell[0]->row = currentRow;
        currentCell[0]->col = currentColumn;
        floodFill(currentCell, 1, 0);
        // cleanup visited
        for (int i = 0; i < unvisitedCount;) {
            if (visited[unvisitedCells[i]->row][unvisitedCells[i]->col]) {
                API_setColor( unvisitedCells[i]->col, unvisitedCells[i]->row,colors[colour]);
                // Swap with the last element
                cell tempCell = unvisitedCells[i];  
                unvisitedCells[i] = unvisitedCells[unvisitedCount - 1]; 
                unvisitedCells[unvisitedCount - 1] = tempCell;
                unvisitedCount--;  // Decrement the count

                // Update quarter counts and weights
                int sixteenth = getSixteenth(unvisitedCells[i]->row, unvisitedCells[i]->col, sixteenthSizeRow, sixteenthSizeCol);
                sixteenthCounts[sixteenth]--; // Decrease count for that sixteenth
                sixteenthWeights[sixteenth] = 32 - sixteenthCounts[sixteenth]; // Recalculate weight
                
                
            }
            else {
                API_setColor( unvisitedCells[i]->col, unvisitedCells[i]->row,colors[BLACK]);
                i++;    
            }
        }

        int nearestIndex = -1;
        int minDistance = INT_MAX;

        // Find the nearest unvisited cell with weighted distance
        for (int i = 0; i < unvisitedCount; i++) {
            int row = unvisitedCells[i]->row;
            int col = unvisitedCells[i]->col;

            // Base distance calculation
            int distance = maze[row][col];

            // Adjust distance based on sixteenth weight
            int sixteenth = getSixteenth(row, col, sixteenthSizeRow, sixteenthSizeCol);
            int currentSixteenth = getSixteenth(currentRow, currentColumn, sixteenthSizeRow, sixteenthSizeCol);
            int proximity = calculateProximity(sixteenth, currentSixteenth);
            int use_weights = proximity > 2 ? 0 : 1;
            distance -=  (sixteenthWeights[sixteenth] * use_weights / 64 - proximity / 16);

            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }


        if (nearestIndex == -1) {
            break; // No unvisited cells left
        }

        cell nearestCell = unvisitedCells[nearestIndex];

        // Use the nearest cell as the new goal
        cell unvisited_goal[1] = {nearestCell};

        // Explore until the nearest unvisited cell is reached
        startExploration(unvisited_goal, 1, colour, 1);
    }

    // Free allocated memory for cells
    for (int i = 0; i < temp; i++) {
        free(unvisitedCells[i]);
    }
    free(currentCell[0]);

}
 
int main() {
    initializeMazeWalls();
    exploreUnvisitedCells();
    
    API_ackReset();
    currentRow = 0;
    currentColumn = 0;
    currentDirection = NORTH;

    
    //2x2 goal cells
    cell goal_cells[4]; // Example goal position
    for(int i = 0; i < 4; i++)
        goal_cells[i] = create_cell(MAX/2 - 1,MAX/2 - 1);

    goal_cells[1]->col = goal_cells[0]->col + 1;

    goal_cells[2]->row = goal_cells[0]->row + 1;

    goal_cells[3]->row = goal_cells[0]->row + 1;
    goal_cells[3]->col = goal_cells[0]->col + 1;

   
    startExploration(goal_cells, 4, BLACK, 0);
    
    API_ackReset();

    for (int i = 0; i < 4; i++){
        free(goal_cells[i]);
    }
    

    return 0;
}
