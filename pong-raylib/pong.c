#include "raylib.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

typedef struct game_object {
    Vector2 position;
    Vector2 velocity;
    Vector2 dimensions;
    Color color;
} GameObj;

bool check_collision(GameObj o1, GameObj o2) {
    return ((o2.position.x >= o1.position.x && o2.position.x <= o1.position.x + o1.dimensions.x) || (o1.position.x >= o2.position.x && o1.position.x <= o2.position.x + o2.dimensions.x)) && ((o2.position.y >= o1.position.y && o2.position.y <= o1.position.y + o1.dimensions.y) || (o1.position.y >= o2.position.y && o1.position.y <= o2.position.y + o2.dimensions.y));
}

// Globals
const int screenWidth = 800;
const int screenHeight = 600;

GameObj ball = {
    .dimensions = {15.0f, 15.0f}, 
    .position = {20.0f, 20.0f},
    .velocity = {300.0f, 300.0f},
    .color = RED
};
GameObj paddle = {
    .dimensions = {100.0f, 20.0f},
    .velocity = {0.0f, 0.0f},
    .position = {350.0f, 560.0f},
    .color = DARKGRAY
};
bool isGameOver = false;

void processInputs(void) {
    if (IsKeyDown(KEY_LEFT)) {
        paddle.velocity.x = -200.0f;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        paddle.velocity.x = 200.0f;
    }
    if (IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_RIGHT)) {
        paddle.velocity.x = 0.0f;
    }
    if (isGameOver && IsKeyReleased(KEY_ENTER)) {
        isGameOver = false;
        ball.velocity = (Vector2) {300.0f, 300.0f};
    }
}

void calculateGame(void) {
    float delta_time = GetFrameTime();
    ball.position.x += ball.velocity.x * delta_time;
    ball.position.y += ball.velocity.y * delta_time;

    // Update paddle position based on its velocity
    paddle.position.x += paddle.velocity.x * delta_time;

    // Check for ball collision with the walls
    if (ball.position.x >= screenWidth - ball.dimensions.x) {
        ball.position.x = screenWidth - ball.dimensions.x;
        ball.velocity.x = -ball.velocity.x;
    }
    if (ball.position.x <= 0) {
        ball.position.x = 0;
        ball.velocity.x = -ball.velocity.x;
    }

    if (ball.position.y <= 0) {
        ball.position.y = 0;
        ball.velocity.y = -ball.velocity.y;
    }

    bool is_collision = check_collision(ball, paddle);

    // Check for ball collision with the paddle
    if (is_collision) {
        ball.velocity.y = -ball.velocity.y;
        ball.position.y = paddle.position.y - ball.dimensions.y;
    }

    // Prevent paddle from moving outside the boundaries of the window
    if (paddle.position.x >= screenWidth - paddle.dimensions.x) {
        paddle.position.x = screenWidth - paddle.dimensions.x;
        paddle.velocity.x = 0;
    }

    if (paddle.position.x <= 0) {
        paddle.position.x = 0;
    }

    // Check for game over when ball hits the bottom part of the screen
    if (!is_collision && ball.position.y >= screenHeight - ball.dimensions.y) {
        ball.velocity = (Vector2) {0.0f, 0.0f};
        ball.position = (Vector2) {60.0f, 160.0f};
        //game_is_running = false;
        isGameOver = true;
    }
}

void renderGame(void) {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleV(ball.position, ball.dimensions, ball.color);
        DrawRectangleV(paddle.position, paddle.dimensions, paddle.color);
        if (GetTime() < 5) {
            DrawText("move the paddle with left and right arrow keys", 20, 20, 20, DARKGRAY);
        }
        if (isGameOver) {
            DrawText("Game Over.\nPress Enter to restart game.\nMove the paddle with left and right arrow keys", 20, 20, 20, DARKGRAY);
        }
    EndDrawing();
}

void runGameLoop(void) {
    processInputs();
    calculateGame();
    renderGame();
}

int main(void) {
    // init
    InitWindow(screenWidth, screenHeight, "raylib pong");
    SetTargetFPS(60);
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