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
    DrawCircle(game->food.x + LEN/2, game->food.y + LEN/2, LEN/2, FOOD_COLOR);
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
    Snake *snake = game->snake;
    static enum Dir next_dir = RIGHT;
    float amplitude;
    if (IsKeyDown(KEY_LEFT_SHIFT)) amplitude = 0.4;
    else amplitude = 1.0;

    if (playing) {

        for (int i = 0; i < game->len; ++i) {
            if (snake[i].x >= SCREEN_WIDTH) snake[i].x -= SCREEN_WIDTH;
            if (snake[i].x < 0) snake[i].x += SCREEN_WIDTH;
            if (snake[i].y >= SCREEN_HEIGHT) snake[i].y -= SCREEN_HEIGHT;
            if (snake[i].y < 0) snake[i].y += SCREEN_HEIGHT;
        }
        const bool in_square = snake[0].x % LEN == 0 && snake[0].y % LEN == 0;

        if (in_square) {
            // Setting direction of current snake to previous snake dir
            for (int i = game->len - 1; i > 0; --i) {
                if (!snake[i].moving) snake[i].moving = true;

                snake[i].dir = snake[i - 1].dir;
            }
            snake[0].dir = next_dir; // Set new direction
        }

        const Snake* head = &snake[0];
        // Eating food
        if (head->x == game->food.x && head->y == game->food.y) {
            game->is_food_available = false;
            const Snake new_tail = {
                .x = snake[game->len - 1].x,
                .y = snake[game->len - 1].y,
                .dir = snake[game->len - 1].dir,
                .moving = false,
            };
            snake[game->len++] = new_tail;
        }

        update_dir(&snake[0].dir, &next_dir);

        for (int i = 0; i < game->len; ++i) {
            if (playing && game->snake[i].moving) {
                if (snake[i].dir == LEFT) game->snake[i].x -= VELOCITY * amplitude;
                if (snake[i].dir == RIGHT) game->snake[i].x += VELOCITY * amplitude;
                if (snake[i].dir == UP) game->snake[i].y -= VELOCITY * amplitude;
                if (snake[i].dir == DOWN) game->snake[i].y += VELOCITY * amplitude;
            }
        }
    }

    for (int i = 0; i < game->len; i++) {
        DrawCircle(snake[i].x+LEN/2, snake[i].y+LEN/2, LEN/2, SNAKE_COLOR);
    }
}


Game setup_game(void) {
    const Game game = {
        .snake = {
            {
                .x = 0,
                .y = 0,
                .dir = RIGHT,
                .moving = true,
            },
        },
        .len = 1,
        .food = get_rand_x(), get_rand_y(),
        .is_food_available = false,
    };
    return game;
}

int main(void) {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake");

    Game game = setup_game();

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
