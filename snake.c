#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define LEN 100
#define MAX_TAIL 100

#define SNAKE_COLOR GetColor(0x3a9124FF)
#define FOOD_COLOR GetColor(0xde6f14FF)

#define VELOCITY 10

static bool playing = true;

typedef struct {
    int32_t x;
    int32_t y;
} IntVector2;

enum Dir {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
};

typedef struct {
    int x;
    int y;
    enum Dir dir;
    bool moving;
} Snake;

typedef struct {
    Snake snake[MAX_TAIL];
    int32_t len;
    IntVector2 food;
    bool is_food_available;
} Game;


void render_background(void)
{
    // Iterate over the screen width and height using the exact pixel dimensions
    for (int i = 0; i < SCREEN_WIDTH; i += LEN) {
        for (int j = 0; j < SCREEN_HEIGHT; j += LEN) {
            // Determine the color based on the sum of the current row and column indices
            const Color color = ((i / LEN + j / LEN) % 2 == 0) ? GetColor(0x666d6e) : GetColor(0x18181818);

            // Draw the rectangle at the appropriate position
            DrawRectangle(i, j, LEN, LEN, color);
        }
    }
}

int get_rand_x(void) {
    return (rand() % 16) * LEN;
}

int get_rand_y(void) {
    return (rand() % 9) * LEN;
}

void render_food(Game* game)
{
    if (!game->is_food_available) {
        game->is_food_available = true;
        game->food.x = get_rand_x();
        game->food.y = get_rand_y();
    }
    DrawRectangle(game->food.x, game->food.y, LEN, LEN, FOOD_COLOR);
}

void update_dir(const enum Dir *dir, enum Dir *next_dir) {
    const bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    const bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    const bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    const bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

    if (left && *dir != RIGHT) *next_dir = LEFT;
    if (right && *dir != LEFT) *next_dir = RIGHT;
    if (up && *dir != DOWN) *next_dir = UP;
    if (down && *dir != UP) *next_dir = DOWN;
}


IntVector2 get_next_pos(const IntVector2 pos, const enum Dir dir) {
    IntVector2 next_pos = pos;
    printf("x: %d y: %d\n", pos.x, pos.y);
    if (dir == LEFT) next_pos.x -= LEN;
    if (dir == RIGHT) next_pos.x += LEN;
    if (dir == UP) next_pos.y -= LEN;
    if (dir == DOWN) next_pos.y += LEN;
    return next_pos;
}

void render_snake(Game* game) {
    static enum Dir dir = RIGHT;
    static enum Dir next_dir = RIGHT;
    static enum Dir last_dir = RIGHT;
    static IntVector2 last_dir_pos = {0, 0};

    for (int i = 0; i < game->len; ++i) {
        if (game->snake[i].x >= SCREEN_WIDTH) game->snake[i].x -= SCREEN_WIDTH;
        if (game->snake[i].x < 0) game->snake[i].x += SCREEN_WIDTH;
        if (game->snake[i].y >= SCREEN_HEIGHT) game->snake[i].y -= SCREEN_HEIGHT;
        if (game->snake[i].y < 0) game->snake[i].y += SCREEN_HEIGHT;
        const bool in_square = game->snake[i].x % LEN == 0 && (int) game->snake[i].y % LEN == 0;

        if (in_square) {
            last_dir = dir; // Save current direction as last direction
            dir = next_dir; // Set new direction
        }

        /*
        for (int i = 0; i < game->len; i++) {
           // printf("x: %d y: %d\n", next_pos.x / SCREEN_WIDTH, next_pos.y / SCREEN_HEIGHT);
           game->tail[i].moving = true;
           const bool tail_in_square = game->tail[i].x % LEN == 0 && game->tail[i].y % LEN == 0;
            if (tail_in_square && game->tail[i].x == next_pos.x && game->tail[i].y == next_pos.y) {
                if (i == 0) {
                    game->tail[i].dir = last_dir;
                }
                else {
                    game->tail[i].dir = game->tail[i - 1].dir;
                }
            }
       }
       */
    }  
    // When in square, render the tail 

    
    // Eating food
    if (game->snake[0].x == game->food.x && game->snake[0].y == game->food.y) {
        game->is_food_available = false;
    }

    update_dir(&dir, &next_dir);

    // We only want to update the direction if the snake is in a square, and we are in playing state
    if (playing) {
        if (dir == LEFT) game->snake[0].x -= VELOCITY;
        if (dir == RIGHT) game->snake[0].x += VELOCITY;
        if (dir == UP) game->snake[0].y -= VELOCITY;
        if (dir == DOWN) game->snake[0].y += VELOCITY;

    }

    for (int i = 0; i < game->len; i++) {
        DrawRectangle(game->snake[i].x, game->snake[i].y, LEN, LEN, SNAKE_COLOR);
    }
}


int main(void) {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

    Game game = {
        .snake = {0, 0},
        .len = 1,
        .food = get_rand_x(), get_rand_y(),
        .is_food_available = false,
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        if (IsKeyPressed(KEY_SPACE)) playing = !playing;

        ClearBackground(BLACK);
        render_background();
        render_food(&game);
        render_snake(&game);

        if (!playing) {
            DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 125, 20, WHITE);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0; 
    
}