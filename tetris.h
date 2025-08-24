#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

// Define constants for the game board and block size
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 30  // pixels
#define NUM_BLOCKS 7

// Structure to represent a Tetris block (tetromino)
typedef struct {
        int shape[4][4]; // 2D array to store the block's shape
        int color_r, color_g, color_b; // RGB color values
        int x, y; // Position on the game board
} Block;

// Enum for the different game states
typedef enum {
        GAME_STATE_MENU,
        GAME_STATE_PLAYING,
        GAME_STATE_GAME_OVER
    } GameState;

// Global variables (declared as extern so they can be used in other files)
extern int board[BOARD_HEIGHT][BOARD_WIDTH];
extern Block currentBlock;
extern int tetrominoes[NUM_BLOCKS][4][4];
extern SDL_Color block_colors[NUM_BLOCKS];
extern int score;
extern GameState gameState;

// Function prototypes
void initBoard();
void drawBoard(SDL_Renderer *renderer);
void spawnBlock();
int checkCollision(int newX, int newY, int newShape[4][4]);
void placeBlock();
void clearLines();
void rotateBlock();
void moveBlock(int dx, int dy);

#endif