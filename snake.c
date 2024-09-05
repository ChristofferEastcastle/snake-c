#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define LEN 100

static bool playing = true;

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

#define SNAKE_COLOR GetColor(0x3a9124FF)
#define FOOD_COLOR GetColor(0xde6f14FF)


enum Dir {
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
};
#define VELOCITY 10

void render_food(void)
{
    static bool is_food_available = false;
    static int rand_x;
    static int rand_y;

    if (!is_food_available) {
        is_food_available = true;
        rand_x = (rand() % 16) * LEN;
        rand_y = (rand()%9) * LEN;
    }
    DrawRectangle(rand_x, rand_y, LEN, LEN, FOOD_COLOR);
}

void render_snake() {
    static Vector2 snake = { 0, 0 };
    static enum Dir dir = RIGHT;
    static enum Dir next_dir = RIGHT;

    if (snake.x >= SCREEN_WIDTH) snake.x -= SCREEN_WIDTH;
    if (snake.x < 0) snake.x += SCREEN_WIDTH;
    if (snake.y >= SCREEN_HEIGHT) snake.y -= SCREEN_HEIGHT;
    if (snake.y < 0) snake.y += SCREEN_HEIGHT;


    const bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    const bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    const bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    const bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

    if (left && dir != RIGHT) next_dir = LEFT;
    if (right && dir != LEFT) next_dir = RIGHT;
    if (up && dir != DOWN) next_dir = UP;
    if (down && dir != UP) next_dir = DOWN;

    if (playing) {
        const bool in_square = (int)snake.x % LEN == 0 && (int) snake.y % LEN == 0;
        if (in_square) dir = next_dir;
        if (dir == LEFT) snake.x -= VELOCITY;
        if (dir == RIGHT) snake.x += VELOCITY;
        if (dir == UP) snake.y -= VELOCITY;
        if (dir == DOWN) snake.y += VELOCITY;
    }

    DrawRectangle(snake.x, snake.y, LEN, LEN, SNAKE_COLOR);
}


int main(void) {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        if (IsKeyPressed(KEY_SPACE)) playing = !playing;

        ClearBackground(BLACK);
        render_background();
        render_food();
        render_snake();

        if(!playing)
            DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 125, 20, WHITE);


        EndDrawing();
    }
    return 0;
}
