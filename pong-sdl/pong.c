#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

///////////////////////////////////////////////////////////////////////////////
// Declare two game objects for the ball and the paddle
///////////////////////////////////////////////////////////////////////////////
struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                game_is_running = false;
            }
            // Set paddle velocity based on left/right arrow keys
            if (event.key.keysym.sym == SDLK_RIGHT) {
            	paddle.vel_x = 200;
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
            	paddle.vel_x = -200;
            }
            break;
        case SDL_KEYUP:
            // Reset paddle velocity based on left/right arrow keys
            if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT) {
            	paddle.vel_x = 0;
            }
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // Initialize the ball object moving down at a constant velocity
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;
    // Initialize the paddle position at the bottom of the screen
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}



bool check_collision(struct game_object o1, struct game_object o2) {
	return ((o2.x >= o1.x && o2.x <= o1.x + o1.width) || (o1.x >= o2.x && o1.x <= o2.x + o2.width)) && ((o2.y >= o1.y && o2.y <= o1.y + o1.height) || (o1.y >= o2.y && o1.y <= o2.y + o2.height));
}


///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
    // Calculate how much we have to wait until we reach the target frame time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast too update this frame
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get delta_time factor converted to seconds to be used to update objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame to be used in the next one
    last_frame_time = SDL_GetTicks();

    // Update ball position based on its velocity
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    // Update paddle position based on its velocity
    paddle.x += paddle.vel_x * delta_time;

    // Check for ball collision with the walls
    if (ball.x >= WINDOW_WIDTH - ball.width) {
    	ball.x = WINDOW_WIDTH - ball.width;
    	ball.vel_x = -ball.vel_x;
    }
    if (ball.x <= 0) {
    	ball.x = 0;
    	ball.vel_x = -ball.vel_x;
    }

    if (ball.y <= 0) {
    	ball.y = 0;
    	ball.vel_y = -ball.vel_y;
    }

    bool is_collision = check_collision(ball, paddle);

    // Check for ball collision with the paddle
    if (is_collision) {
    	ball.vel_y = -ball.vel_y;
    	ball.y = paddle.y - ball.height;
    }

    // Prevent paddle from moving outside the boundaries of the window
    if (paddle.x >= WINDOW_WIDTH - paddle.width) {
    	paddle.x = WINDOW_WIDTH - paddle.width;
    	paddle.vel_x = 0;
    }

    if (paddle.x <= 0) {
    	paddle.x = 0;
    	paddle.vel_x = paddle.vel_x;
    }

    // TODO: Check for game over when ball hits the bottom part of the screen
    if (!is_collision && ball.y >= WINDOW_HEIGHT - ball.height) {
    	ball.vel_x = 0;
    	ball.vel_y = 0;
    	ball.x = 400;
    	ball.y = 400;
    	//game_is_running = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a rectangle for the paddle object
    SDL_Rect paddle_rect = {
        (int)paddle.x,
        (int)paddle.y,
        (int)paddle.width,
        (int)paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
    #endif
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////

void main_loop(void) {
	process_input();
	update();
	render();
}

int main(int argc, char* args[]) {
	game_is_running = initialize_window();
    setup();
    #ifndef __EMSCRIPTEN__
    while (game_is_running) {
        main_loop();
    }
    #endif
    #ifdef __EMSCRIPTEN__
    	emscripten_set_main_loop(main_loop, 0, 1);
    #endif
    destroy_window();
}
