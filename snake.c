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
} Tail;


typedef struct {
    Vector2 snake;
    Tail tail[MAX_TAIL];
    int tail_length;
    Vector2 food;
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


Vector2 get_next_pos(Vector2 pos, enum Dir dir) {
    Vector2 next_pos = pos;
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
    static Vector2 last_dir_pos = {0, 0};

    if (game->snake.x >= SCREEN_WIDTH) game->snake.x -= SCREEN_WIDTH;
    if (game->snake.x < 0) game->snake.x += SCREEN_WIDTH;
    if (game->snake.y >= SCREEN_HEIGHT) game->snake.y -= SCREEN_HEIGHT;
    if (game->snake.y < 0) game->snake.y += SCREEN_HEIGHT;


    const bool in_square = (int)game->snake.x % LEN == 0 && (int) game->snake.y % LEN == 0;
    if (in_square) {
        last_dir = dir; // Save current direction as last direction
        dir = next_dir; // Set new direction
        Vector2 next_pos = get_next_pos(game->snake, next_dir); // Save current position as last position 
        // create a function that calculates the next position of snake. 


        for (int i = 0; i < game->tail_length; i++) {
           // printf("x: %d y: %d\n", next_pos.x / SCREEN_WIDTH, next_pos.y / SCREEN_HEIGHT);
           game->tail[i].moving = true;
           const bool in_square = (int)game->tail[i].x % LEN == 0 && (int) game->tail[i].y % LEN == 0;
            if (in_square && game->tail[i].x == next_pos.x && game->tail[i].y == next_pos.y) {
                if (i == 0) {
                    game->tail[i].dir = last_dir;
                }
                else {
                    game->tail[i].dir = game->tail[i - 1].dir;
                }
            }
       }
    }  
    // When in square, render the tail 

    
    // Eating food
    if (game->snake.x == game->food.x && game->snake.y == game->food.y) {
        game->is_food_available = false;
        Tail tail = {.x = game->snake.x, .y = game->snake.y, dir, .moving = false};
        game->tail[game->tail_length++] = tail;
    }

    update_dir(&dir, &next_dir);

    // We only want to update the direction if the snake is in a square, and we are in playing state
    if (playing) {

        if (dir == LEFT) game->snake.x -= VELOCITY;
        if (dir == RIGHT) game->snake.x += VELOCITY;
        if (dir == UP) game->snake.y -= VELOCITY;
        if (dir == DOWN) game->snake.y += VELOCITY;

        for (int i = 0; i < game->tail_length; i++) {
            if (!game->tail[i].moving) continue;

            
            if (dir == LEFT) game->tail[i].x -= VELOCITY;
            if (dir == RIGHT) game->tail[i].x += VELOCITY;
            if (dir == UP) game->tail[i].y -= VELOCITY;
            if (dir == DOWN) game->tail[i].y += VELOCITY;
        }
    }

    DrawRectangle(game->snake.x, game->snake.y, LEN, LEN, SNAKE_COLOR);

    for (int i = 0; i < game->tail_length; i++) {
        DrawRectangle(game->tail[i].x, game->tail[i].y, LEN, LEN, SNAKE_COLOR);
    }
    
}


int main(void) {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

    Game game = {
        .snake = {0, 0},
        .tail = {0},
        .tail_length = 0,
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