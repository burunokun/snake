#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL2/SDL.h"

#define CELL 20
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define COLOR_GRID       0xFF202020
#define COLOR_APPLE      0xFFFF0000
#define COLOR_SNAKE      0xFFFFFFFF
#define COLOR_BACKGROUND 0x00000000

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
} Dir;

typedef struct {
    int x, y;
} Node;

typedef struct {
    Node *body;
    Dir dir;
    int x, y;
    int len;
} Snake;

void node_alloc(Snake *s) {
    s->body = malloc(sizeof(*s->body)*s->len);
    assert(s->body != NULL);
}

void draw_snake(SDL_Surface *surface, Snake *s) {
    SDL_Rect snake = {
        .x = s->x, .y = s->y,
        .w = CELL, .h = CELL,
    };
    SDL_FillRect(surface, &snake, COLOR_SNAKE);

    for (int i = 1; i < s->len; ++i) {
        SDL_Rect body = {
            .x = s->body[i].x, .y = s->body[i].y,
            .w = CELL, .h = CELL,
        };
        SDL_FillRect(surface, &body, COLOR_SNAKE);
    }
}

void update_snake(Snake *s) {
    s->len += 1;
    Node *temp = realloc(s->body, sizeof(*s->body)*s->len);
    assert(temp != NULL);
    s->body = temp;
    s->body[s->len-1] = (Node) {.x = s->x, .y = s->y};

}

void move_snake(Snake *s) {
    for (int i = 0; i < s->len-1; i++) {
        s->body[i] = s->body[i+1];
    }
    s->body[s->len-1] = (Node) {.x = s->x, .y = s->y};

    switch (s->dir) {
        case UP:    s->y -= CELL; break;
        case DOWN:  s->y += CELL; break;
        case LEFT:  s->x -= CELL; break;
        case RIGHT: s->x += CELL; break;
        default:
    }

    if (s->x < 0) s->x = SCREEN_WIDTH;
    if (s->x > SCREEN_WIDTH) s->x = 0;
    if (s->y < 0) s->y = SCREEN_HEIGHT;
    if (s->y > SCREEN_HEIGHT) s->y = 0;

}

bool has_hit_itself(Snake *s) {
    bool res = false;

    for (int i = 0; i < s->len-1; i++) {
        if (s->x == s->body[i].x && s->y == s->body[i].y) {
            res = !res;
        }
    }

    return res;
}

void generate_apple(SDL_Rect *a, Snake *s) {
    srand(time(NULL));
    int nx = 0;
    int ny = 0;
    bool is_valid = false;

    do {
        nx = (rand()%SCREEN_WIDTH/CELL)*CELL;
        ny = (rand()%SCREEN_HEIGHT/CELL)*CELL;
        for (int i = 0; i < s->len; ++i) {
            if (nx != s->body[i].x && ny != s->body[i].y) is_valid = !is_valid;
        }
    } while (!is_valid);

    *a = (SDL_Rect) {
        .x = nx,
        .y = ny,
        .w = CELL,
        .h = CELL,
    };

}

void draw_grid(SDL_Surface *surface) {
    SDL_Rect grid;

    grid.y = 0;
    grid.w = 1;
    grid.h = SCREEN_HEIGHT;
    for (grid.x = 0; grid.x < SCREEN_WIDTH; grid.x += CELL) {
        SDL_FillRect(surface, &grid, COLOR_GRID);
    }

    grid.x = 0;
    grid.w = SCREEN_WIDTH;
    grid.h = 1;
    for (grid.y = 0; grid.y < SCREEN_HEIGHT; grid.y += CELL) {
        SDL_FillRect(surface, &grid, COLOR_GRID);
    }
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Snake",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return -1;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface) return -1;

    Snake snake = {0};
    snake.dir = RIGHT;
    snake.len = 3;
    snake.x = CELL;
    snake.y = 3*CELL;
    node_alloc(&snake);

    SDL_Rect apple;
    generate_apple(&apple, &snake);

    SDL_Rect clear = {
        .x = 0,
        .y = 0,
        .w = SCREEN_WIDTH,
        .h = SCREEN_HEIGHT,
    };

    bool quit = false;
    bool paused = false;
    SDL_Event ev;

    while (!quit) {

        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    quit = !quit;
                    break;
                case SDL_KEYDOWN:
                    if (ev.key.keysym.sym == SDLK_ESCAPE) quit = !quit;;
                    if (ev.key.keysym.sym == SDLK_SPACE)  paused = !paused;
                    if (!paused) {
                        if (snake.dir != UP &&
                            ev.key.keysym.sym == SDLK_j) snake.dir = DOWN;
                        else if (snake.dir != DOWN &&
                            ev.key.keysym.sym == SDLK_k) snake.dir = UP;
                        else if (snake.dir != RIGHT &&
                            ev.key.keysym.sym == SDLK_h) snake.dir = LEFT;
                        else if (snake.dir != LEFT &&
                            ev.key.keysym.sym == SDLK_l) snake.dir = RIGHT;
                    }
                    break;
            }
        }

        if (!paused) {
            SDL_FillRect(surface, &clear, COLOR_BACKGROUND);

            move_snake(&snake);
            if (snake.x == apple.x && snake.y == apple.y) {
                generate_apple(&apple, &snake);
                update_snake(&snake);
            }

            if (has_hit_itself(&snake)) quit = !quit;

            draw_snake(surface, &snake);
            SDL_FillRect(surface, &apple, COLOR_APPLE);

            draw_grid(surface);
            SDL_UpdateWindowSurface(window);
        }

        SDL_Delay(120);
    }

    free(snake.body);

    SDL_DestroyWindow(window);
    surface = NULL;
    window = NULL;

    SDL_Quit();

    return 0;
}
