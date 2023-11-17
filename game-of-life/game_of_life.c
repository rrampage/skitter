#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#define MAT_ROWS 512
#define MAT_COLS 512

// Globals
const int screenWidth = 800;
const int screenHeight = 800;
const Color liveColor = GOLD;
const Color deadColor = DARKPURPLE;
const float cellHeight = (screenHeight + 0.0f)/MAT_ROWS;
const float cellWidth = (screenWidth + 0.0f)/MAT_COLS;
Vector2 cellSize = {cellHeight, cellWidth};

// toggle pause with Enter
bool isPaused = false;
int iterNum = 0;
char fpsTextBuffer[50];
char matrixSizeBuffer[50];
char liveCellCountBuffer[50];
// backing matrix
char matrix[MAT_ROWS][MAT_COLS];

// xoshiro PRNG from https://en.wikipedia.org/wiki/Xorshift#xoshiro256**

uint64_t rol64(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

struct xoshiro256ss_state {
    uint64_t s[4];
};

uint64_t xoshiro256ss(struct xoshiro256ss_state *state) {
    uint64_t *s = state->s;
    uint64_t const result = rol64(s[1] * 5, 7) * 9;
    uint64_t const t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;
    s[3] = rol64(s[3], 45);

    return result;
}

struct xoshiro256ss_state seed;

void seedMatrix() {
    int liveCells = 0;
    iterNum = 0;
    uint64_t timeStamp = (uint64_t)time(NULL);
    seed.s[0] = timeStamp;
    seed.s[1] = timeStamp;
    seed.s[2] = 0xBF58476D1CE4E5B9;
    seed.s[3] = timeStamp;
    for (int i = 0; i < MAT_ROWS; i++) {
        for (int j = 0; j < MAT_COLS; j++) {
            uint64_t res = xoshiro256ss(&seed);
            if (res % 256 > 127) {
                matrix[i][j] = 1;
                liveCells++;
            }
        }
    }
    sprintf(fpsTextBuffer, "Iterations: %d" , iterNum);
    sprintf(liveCellCountBuffer, "Live: %d / %d" , liveCells, MAT_ROWS*MAT_COLS);
}

int safe_val(int i, int j) {
    return ( i < 0 || i >= MAT_ROWS || j < 0 || j >= MAT_COLS ) ? 0 : matrix[i][j]%2;
}

void calculateGameOfLife(void) {
    int liveCells = 0;
    for (int i = 0; i < MAT_ROWS; i++) {
        for (int j = 0; j < MAT_COLS; j++) {
            int liveCells = safe_val(i-1,j-1) 
            + safe_val(i-1, j)
            + safe_val(i-1, j+1)
            + safe_val(i,j-1)
            + safe_val(i, j+1)
            + safe_val(i+1, j-1)
            + safe_val(i+1, j)
            + safe_val(i+1, j+1);
            //printf("Live Cells for %d %d = %d\n", i, j, liveCells);
            bool isLive = (matrix[i][j] == 1) ? (liveCells == 2 || liveCells == 3) : (liveCells == 3);
            matrix[i][j] = (isLive) ? 2+matrix[i][j] : matrix[i][j];
        }
    }
    for (int i = 0; i < MAT_ROWS; i++) {
        for (int j = 0; j < MAT_COLS; j++) {
            matrix[i][j] = matrix[i][j] >> 1;
            if (matrix[i][j]) {
                liveCells++;
            }
        }
    }
    iterNum++;
    sprintf(fpsTextBuffer, "Iterations: %d" , iterNum);
    sprintf(liveCellCountBuffer, "Live: %05d / %d" , liveCells, MAT_ROWS*MAT_COLS);
}

void processInputs(void) {
    if (IsKeyReleased(KEY_ENTER)) {
        // toggle Pause
        isPaused = !isPaused;
    }
    if (IsKeyReleased(KEY_R)) {
        // re-seed matrix
        seedMatrix();
    }
}

void renderGame(void) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < MAT_ROWS; i++) {
            for (int j = 0; j < MAT_COLS; j++) {
                DrawRectangleV((Vector2){cellHeight * i, cellWidth*j}, cellSize, (matrix[i][j]) ? liveColor : deadColor);
            }
        }
        DrawText(fpsTextBuffer, 620, 20, 20, RAYWHITE);
        DrawText(matrixSizeBuffer, 620, 50, 10, RAYWHITE);
        DrawText(liveCellCountBuffer, 620, 60, 10, RAYWHITE);
        if (isPaused || GetTime() < 3) {
            DrawText("Press Enter to pause/unpause and R to re-run simulation", 120, 320, 20, RAYWHITE);
        }
    EndDrawing();
}

void runGameLoop(void) {
    processInputs();
    if (!isPaused) {
        calculateGameOfLife();
    }
    renderGame();
}

int main(void) {
    // init
    InitWindow(screenWidth, screenHeight, "raylib pong");
    SetTargetFPS(60);
    seedMatrix();
    sprintf(matrixSizeBuffer, "Grid size: %d x %d" , MAT_ROWS, MAT_COLS);
    #ifndef __EMSCRIPTEN__
    while (!WindowShouldClose()) {
        runGameLoop();
    }
    #endif
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(runGameLoop, 0, 1);
    #endif
    CloseWindow();
    #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
    #endif
    return 0;
}