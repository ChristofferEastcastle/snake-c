#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define LEN 100
#define MAX_TAIL 100
#define EYE_SIZE 6
#define TONGUE_WIDTH (LEN / 4)
#define TONGUE_LENGTH 10
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


void render_background(void) {
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

void render_food(Game *game) {
    if (!game->is_food_available) {
        game->is_food_available = true;
        game->food.x = get_rand_x();
        game->food.y = get_rand_y();
    }
    DrawCircle(game->food.x + LEN / 2, game->food.y + LEN / 2, LEN / 2, FOOD_COLOR);
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

void eat_food(Game *game, Snake *snake) {
    game->is_food_available = false;
    const Snake new_tail = {
        .x = snake[game->len - 1].x,
        .y = snake[game->len - 1].y,
        .dir = snake[game->len - 1].dir,
        .moving = false,
    };
    snake[game->len++] = new_tail;
}

void add_velocity(Game *game) {
    const Snake *snake = game->snake;
    for (int i= 0; i < game->len; ++i) {
        if (playing && game->snake[i].moving) {
            if (snake[i].dir == LEFT) game->snake[i].x -= VELOCITY;
            if (snake[i].dir == RIGHT) game->snake[i].x += VELOCITY;
            if (snake[i].dir == UP) game->snake[i].y -= VELOCITY;
            if (snake[i].dir == DOWN) game->snake[i].y += VELOCITY;
        }
    }
}

void draw_snake_body(const Game *game) {
    for (int i = 0; i < game->len; i++) {
        DrawCircle(game->snake[i].x + LEN / 2, game->snake[i].y + LEN / 2, LEN / 2, SNAKE_COLOR);
        if (i == 0) {
            int eye_x1, eye_y1, eye_x2, eye_y2, tongue_x, tongue_y, tongue_w, tongue_h;
            switch (game->snake[i].dir) {
                case LEFT:
                    eye_x1 = eye_x2 = game->snake[i].x + LEN / 4;
                    eye_y1 = game->snake[i].y + LEN / 3;
                    eye_y2 = game->snake[i].y + LEN / 4 * 3;
                    tongue_x = game->snake[i].x - TONGUE_WIDTH;
                    tongue_y = game->snake[i].y + LEN / 2 - 5;
                    tongue_w = TONGUE_WIDTH;
                    tongue_h = TONGUE_LENGTH;
                    break;
                case RIGHT:
                    eye_x1 = eye_x2 = game->snake[i].x + LEN / 4 * 3;
                    eye_y1 = game->snake[i].y + LEN / 3;
                    eye_y2 = game->snake[i].y + LEN / 4 * 3;
                    tongue_x = game->snake[i].x + LEN;
                    tongue_y = game->snake[i].y + LEN / 2 - 5;
                    tongue_w = TONGUE_WIDTH;
                    tongue_h = TONGUE_LENGTH;
                    break;
                case UP:
                    eye_x1 = game->snake[i].x + LEN / 3;
                    eye_x2 = game->snake[i].x + LEN / 4 * 3;
                    eye_y1 = eye_y2 = game->snake[i].y + LEN / 4;
                    tongue_x = game->snake[i].x + LEN / 2 - 5;
                    tongue_y = game->snake[i].y - TONGUE_WIDTH;
                    tongue_w = TONGUE_LENGTH;
                    tongue_h = TONGUE_WIDTH;
                    break;
                case DOWN:
                    eye_x1 = game->snake[i].x + LEN / 3;
                    eye_x2 = game->snake[i].x + LEN / 4 * 3;
                    eye_y1 = eye_y2 = game->snake[i].y + LEN / 4 * 3;
                    tongue_x = game->snake[i].x + LEN / 2 - 5;
                    tongue_y = game->snake[i].y + LEN;
                    tongue_w = TONGUE_LENGTH;
                    tongue_h = TONGUE_WIDTH;
                    break;
                default:
                    // Setting all to 10 to avoid warnings
                    eye_x1 = eye_y1 = eye_x2 = eye_y2 = tongue_x = tongue_y = tongue_w = tongue_h = 10;
            }
            DrawCircle(eye_x1, eye_y1, EYE_SIZE, BLACK);
            DrawCircle(eye_x2, eye_y2, EYE_SIZE, BLACK);
            DrawRectangle(tongue_x, tongue_y, tongue_w, tongue_h, RED);
        }
    }
}

void render_snake(Game *game) {
    static enum Dir next_dir = RIGHT;

    if (playing) {
        for (int i = 0; i < game->len; ++i) {
            if (game->snake[i].x >= SCREEN_WIDTH) game->snake[i].x -= SCREEN_WIDTH;
            if (game->snake[i].x < 0) game->snake[i].x += SCREEN_WIDTH;
            if (game->snake[i].y >= SCREEN_HEIGHT) game->snake[i].y -= SCREEN_HEIGHT;
            if (game->snake[i].y < 0) game->snake[i].y += SCREEN_HEIGHT;
        }
        const bool in_square = game->snake[0].x % LEN == 0 && game->snake[0].y % LEN == 0;

        if (in_square) {
            // Setting direction of current tail to previous snake head direction
            for (int i = game->len - 1; i > 0; --i) {
                if (!game->snake[i].moving) game->snake[i].moving = true;
                game->snake[i].dir = game->snake[i - 1].dir;
            }
            game->snake[0].dir = next_dir; // Set new direction to head
        }

        // Check if snake head is intersecting with food
        if (game->snake[0].x == game->food.x && game->snake[0].y == game->food.y) {
            eat_food(game, game->snake);
        }
        update_dir(&game->snake[0].dir, &next_dir);
        add_velocity(game);
    }
    draw_snake_body(game);
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

void render_score_and_fps(const Game game) {
    char score[20];
    sprintf(score, "Score:\t%d", game.len - 1);
    const int pos_x = SCREEN_WIDTH -LEN+10;
    DrawText(score, pos_x, 50, 18, GRAY);
    DrawFPS(pos_x, 10);
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

        render_score_and_fps(game);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
