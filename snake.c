#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define LEN 100

static bool playing = true;

typedef struct {
    Vector2 snake;
    Vector2 food;
    bool is_food_available;
} Game;

#define SNAKE_COLOR GetColor(0x3a9124FF)
#define FOOD_COLOR GetColor(0xde6f14FF)


enum Dir {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
};
#define VELOCITY 10

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

void render_snake(Game* game) {
    static enum Dir dir = RIGHT;
    static enum Dir next_dir = RIGHT;

    if (game->snake.x >= SCREEN_WIDTH) game->snake.x -= SCREEN_WIDTH;
    if (game->snake.x < 0) game->snake.x += SCREEN_WIDTH;
    if (game->snake.y >= SCREEN_HEIGHT) game->snake.y -= SCREEN_HEIGHT;
    if (game->snake.y < 0) game->snake.y += SCREEN_HEIGHT;


    const bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    const bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    const bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    const bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

    if (left && dir != RIGHT) next_dir = LEFT;
    if (right && dir != LEFT) next_dir = RIGHT;
    if (up && dir != DOWN) next_dir = UP;
    if (down && dir != UP) next_dir = DOWN;

    // We only want to update the direction if the snake is in a square and we are in playing state
    if (playing) {
        const bool in_square = (int)game->snake.x % LEN == 0 && (int) game->snake.y % LEN == 0;
        if (in_square) dir = next_dir;
        if (dir == LEFT) game->snake.x -= VELOCITY;
        if (dir == RIGHT) game->snake.x += VELOCITY;
        if (dir == UP) game->snake.y -= VELOCITY;
        if (dir == DOWN) game->snake.y += VELOCITY;
    }

    if (game->snake.x == game->food.x && game->snake.y == game->food.y) {
        game->is_food_available = false;

    }

    DrawRectangle(game->snake.x, game->snake.y, LEN, LEN, SNAKE_COLOR);
}


int main(void) {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

    Game game = {
        .snake = {0, 0},
        .food = get_rand_x(), get_rand_y(),
        .is_food_available = false,
    };

    while (!WindowShouldClose())
    {
        BeginDrawing();
        if (IsKeyPressed(KEY_SPACE)) playing = !playing;

        ClearBackground(BLACK);
        render_background();
        render_food(&game);
        render_snake(&game);

        if(!playing)
            DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 125, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
