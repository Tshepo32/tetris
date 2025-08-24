#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "tetris.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

// Global SDL variables
SDL_Window *window;
SDL_Renderer *renderer;
int running = 1;

// Global font for score display
TTF_Font *font;
GameState gameState = GAME_STATE_MENU; // Initial game state is the menu

// Rectangles for the buttons
SDL_Rect startButtonRect = {0, 0, 150, 50};
SDL_Rect restartButtonRect = {0, 0, 150, 50};

// Function to draw the score on the screen
void drawScore(SDL_Renderer *renderer) {
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);

    SDL_Color textColor = {255, 255, 255, 255}; // white
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect;
    textRect.x = BOARD_WIDTH * BLOCK_SIZE + 10;
    textRect.y = 10;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

// Function to draw a button with text
void drawButton(SDL_Renderer *renderer, const char *text, SDL_Rect *rect) {
    // Draw button background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Dark grey
    SDL_RenderFillRect(renderer, rect);

    // Draw button border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_RenderDrawRect(renderer, rect);

    // Draw button text
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = rect->x + (rect->w - textRect.w) / 2;
    textRect.y = rect->y + (rect->h - textRect.h) / 2;

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

// Function to draw the main menu screen
void drawMenu(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Position the start button
    startButtonRect.x = (BOARD_WIDTH * BLOCK_SIZE + 150) / 2 - startButtonRect.w / 2;
    startButtonRect.y = (BOARD_HEIGHT * BLOCK_SIZE) / 2 - startButtonRect.h / 2;

    drawButton(renderer, "Start", &startButtonRect);

    SDL_RenderPresent(renderer);
}

// Function to draw the game over screen
void drawGameOver(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Position the restart button
    restartButtonRect.x = (BOARD_WIDTH * BLOCK_SIZE + 150) / 2 - restartButtonRect.w / 2;
    restartButtonRect.y = (BOARD_HEIGHT * BLOCK_SIZE) / 2 + 10;

    drawButton(renderer, "Play Again", &restartButtonRect);
    drawScore(renderer); // Show the final score

    SDL_RenderPresent(renderer);
}


// Main game loop
void game_loop() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) { // Poll for events (keyboard, quit, etc.)
        if (e.type == SDL_QUIT) {
            running = 0; // End the game loop
            break;
        }

        // Handle button clicks
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mousePoint = { e.button.x, e.button.y };

            if (gameState == GAME_STATE_MENU) {
                if (SDL_PointInRect(&mousePoint, &startButtonRect)) {
                    gameState = GAME_STATE_PLAYING;
                    initBoard();
                    score = 0;
                    spawnBlock();
                }
            } else if (gameState == GAME_STATE_GAME_OVER) {
                if (SDL_PointInRect(&mousePoint, &restartButtonRect)) {
                    gameState = GAME_STATE_PLAYING;
                    initBoard();
                    score = 0;
                    spawnBlock();
                }
            }
        }

        // Handle player input during gameplay (keyboard controls are kept)
        if (e.type == SDL_KEYDOWN) {
            if (gameState == GAME_STATE_PLAYING) {
                switch (e.key.keysym.sym) {
                    case SDLK_a:
                    case SDLK_LEFT: moveBlock(-1, 0); break;
                    case SDLK_d:
                    case SDLK_RIGHT: moveBlock(1, 0); break;
                    case SDLK_s:
                    case SDLK_DOWN: moveBlock(0, 1); break;
                    case SDLK_w:
                    case SDLK_UP: rotateBlock(); break;
                }
            }
        }
    }

    // Update and draw the screen based on the current game state
    if (gameState == GAME_STATE_PLAYING) {
        static int counter = 0;
        counter++;
        if (counter >= 30) { // roughly every 0.5 sec at 60fps
            moveBlock(0, 1); // Automatic fall
            counter = 0;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawBoard(renderer);
        drawScore(renderer);
        SDL_RenderPresent(renderer);
    } else if (gameState == GAME_STATE_MENU) {
        drawMenu(renderer);
    } else if (gameState == GAME_STATE_GAME_OVER) {
        drawGameOver(renderer);
    }

    if (!running) {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        // Clean up resources when the game exits
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed the random number generator

    // Initialize SDL video and TTF
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Create the game window and renderer
    window = SDL_CreateWindow(
        "Tetris",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        BOARD_WIDTH * BLOCK_SIZE + 150, BOARD_HEIGHT * BLOCK_SIZE,
        0
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    font = TTF_OpenFont("DejaVuSansMono.ttf", 24);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initial game setup
    initBoard();
    spawnBlock();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(game_loop, 0, 1); // browser handles loop timing
#else
    while (running) {
        game_loop();
        SDL_Delay(16); // ~60 FPS for native build
    }
#endif

    return 0;
}