#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define LEN 120

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

int main(void) {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");


    Vector2 snake = { 0, 0 };
    enum Dir dir = RIGHT;
    enum Dir next_dir = RIGHT;
    bool playing = true;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);
        render_background();
        render_food();

        if (IsKeyPressed(KEY_SPACE)) playing = !playing;


        if (snake.x > SCREEN_WIDTH) snake.x = 0;
        if (snake.x + LEN < 0) snake.x = SCREEN_WIDTH;
        if (snake.y > SCREEN_HEIGHT) snake.y = 0;
        if (snake.y + LEN < 0) snake.y = SCREEN_HEIGHT;


        const bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        const bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        const bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        const bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

        if (left) next_dir = LEFT;
        if (right) next_dir = RIGHT;
        if (up) next_dir = UP;
        if (down) next_dir = DOWN;


        const bool in_square = (int)snake.x % LEN == 0 && (int) snake.y % LEN == 0;
        if (in_square) dir = next_dir;


        if (playing)
        {
            if (dir == LEFT) snake.x -= VELOCITY;
            if (dir == RIGHT) snake.x += VELOCITY;
            if (dir == UP) snake.y -= VELOCITY;
            if (dir == DOWN) snake.y += VELOCITY;
        }
        else DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 125, 20, WHITE);
        DrawRectangle(snake.x, snake.y, LEN, LEN, SNAKE_COLOR);


        EndDrawing();
    }
    return 0;
}
