#include "tetris.h"

// Example Tetromino shapes (I, O, T, S, Z, J, L)
int tetrominoes[NUM_BLOCKS][4][4] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // I-shape
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // O-shape
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // T-shape
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // S-shape
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, // Z-shape
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // J-shape
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}  // L-shape
};

// Colors for each Tetromino shape (in RGBA)
SDL_Color block_colors[NUM_BLOCKS] = {
    {0, 255, 255, 255},   // I (Cyan)
    {255, 255, 0, 255},   // O (Yellow)
    {128, 0, 128, 255},   // T (Purple)
    {0, 255, 0, 255},     // S (Green)
    {255, 0, 0, 255},     // Z (Red)
    {0, 0, 255, 255},     // J (Blue)
    {255, 165, 0, 255}    // L (Orange)
};

// Global variables defined here
int board[BOARD_HEIGHT][BOARD_WIDTH];
Block currentBlock;
int score = 0;

// Function to initialize the game board with empty cells (0)
void initBoard() {
    for (int i = 0; i < BOARD_HEIGHT; i++)
        for (int j = 0; j < BOARD_WIDTH; j++)
            board[i][j] = 0;
}

// Function to draw the game board and the current falling block
void drawBoard(SDL_Renderer *renderer) {
    // Draw blocks on board
    SDL_Rect rect;
    rect.w = BLOCK_SIZE - 2;
    rect.h = BLOCK_SIZE - 2;

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j]) {
                SDL_Color color = block_colors[board[i][j] - 1];
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                rect.x = j * BLOCK_SIZE + 1;
                rect.y = i * BLOCK_SIZE + 1;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    // Draw the current block
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentBlock.shape[i][j]) {
                SDL_SetRenderDrawColor(renderer, currentBlock.color_r, currentBlock.color_g, currentBlock.color_b, 255);
                rect.x = (currentBlock.x + j) * BLOCK_SIZE + 1;
                rect.y = (currentBlock.y + i) * BLOCK_SIZE + 1;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

// Function to spawn a new block at the top of the board
void spawnBlock() {
    int r = rand() % NUM_BLOCKS; // Get a random block type
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            currentBlock.shape[i][j] = tetrominoes[r][i][j];

    // Assign color and position
    currentBlock.color_r = block_colors[r].r;
    currentBlock.color_g = block_colors[r].g;
    currentBlock.color_b = block_colors[r].b;
    currentBlock.x = BOARD_WIDTH / 2 - 2;
    currentBlock.y = 0;

    // Check for game over condition (spawning in a occupied spot)
    if (checkCollision(currentBlock.x, currentBlock.y, currentBlock.shape)) {
        gameState = GAME_STATE_GAME_OVER;
    }
}

// Function to check for collisions with walls or other blocks
int checkCollision(int newX, int newY, int newShape[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (newShape[i][j]) {
                int x = newX + j;
                int y = newY + i;
                // Check if outside board boundaries or if position is occupied
                if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT || (y >= 0 && board[y][x]))
                    return 1;
            }
        }
    }
    return 0;
}

// Function to place the current block onto the board permanently
void placeBlock() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentBlock.shape[i][j]) {
                int color_index = 0;
                // Find the color index to store on the board
                for (int k = 0; k < NUM_BLOCKS; k++) {
                    if (block_colors[k].r == currentBlock.color_r &&
                        block_colors[k].g == currentBlock.color_g &&
                        block_colors[k].b == currentBlock.color_b) {
                        color_index = k + 1;
                        break;
                    }
                }
                board[currentBlock.y + i][currentBlock.x + j] = color_index;
            }
    clearLines();
    spawnBlock();
}

// Function to check and clear full lines
void clearLines() {
    int lines_cleared = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        int full = 1;
        for (int j = 0; j < BOARD_WIDTH; j++)
            if (!board[i][j]) {
                full = 0;
                break;
            }
        if (full) {
            lines_cleared++;
            // Shift all lines above down by one
            for (int k = i; k > 0; k--)
                for (int j = 0; j < BOARD_WIDTH; j++)
                    board[k][j] = board[k-1][j];
            // Clear the top line
            for (int j = 0; j < BOARD_WIDTH; j++) board[0][j] = 0;
        }
    }
    score += lines_cleared * lines_cleared * 100; // Scoring formula
}

// Function to rotate the current block clockwise
void rotateBlock() {
    int newShape[4][4] = {0};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            newShape[j][3 - i] = currentBlock.shape[i][j]; // Rotation logic

    // If no collision after rotation, apply the new shape
    if (!checkCollision(currentBlock.x, currentBlock.y, newShape))
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                currentBlock.shape[i][j] = newShape[i][j];
}

// Function to move the current block
void moveBlock(int dx, int dy) {
    int newX = currentBlock.x + dx;
    int newY = currentBlock.y + dy;
    if (!checkCollision(newX, newY, currentBlock.shape)) {
        currentBlock.x = newX;
        currentBlock.y = newY;
    } else if (dy == 1) { // If collision on the Y axis, place the block
        placeBlock();
    }
}