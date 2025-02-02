#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL2/SDL.h"

#define ZOOM 1
#define CELL (20*ZOOM)
#define SCREEN_WIDTH (320*ZOOM)
#define SCREEN_HEIGHT (240*ZOOM)

#define SNAKE_INIT_DIR RIGHT
#define SNAKE_INIT_LEN 2
#define SNAKE_INIT_CAP 10
#define SNAKE_INIT_X ((SCREEN_WIDTH/2)-CELL)
#define SNAKE_INIT_Y ((SCREEN_HEIGHT/2)-CELL)

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
    int cap;
} Snake;

typedef struct {
    uint8_t r; 
    uint8_t g; 
    uint8_t b; 
    uint8_t a; 
} Color;

Color lerp(Color c1, Color c2, float t) {
    return (Color) {
        .r = c1.r + (c2.r - c1.r) * t,
        .g = c1.g + (c2.g - c1.g) * t,
        .b = c1.b + (c2.b - c1.b) * t,
        .a = c1.a + (c2.a - c1.a) * t,
    };
}

float normalize(int n, int min, int max) {
    return ((float)n - (float)min) / ((float)max - (float)min);
}

void swap(Snake *s) {
    int next_x = s->body[0].x;
    int next_y = s->body[0].y;

    s->body[0].x = s->x;
    s->body[0].y = s->y;

    int temp_x = 0;
    int temp_y = 0;

     for (int i = 1; i < s->len; ++i) {
         temp_x = s->body[i].x;
         temp_y = s->body[i].y;

         s->body[i].x = next_x;
         s->body[i].y = next_y;

         next_x = temp_x;
         next_y = temp_y;
     }
}

void node_alloc(Snake *s) {
    s->body = malloc(sizeof(*s->body)*s->cap);
    assert(s->body != NULL);
    swap(s);
}

void node_free(Snake *s) {
    s->cap = 0;
    s->body = realloc(s->body, s->cap);
    free(s->body);
    assert(s->body == NULL);
}

void init_snake(Snake *s) {
    s->dir = SNAKE_INIT_DIR;
    s->len = SNAKE_INIT_LEN;
    s->cap = SNAKE_INIT_CAP;
    s->x = SNAKE_INIT_X;
    s->y = SNAKE_INIT_Y;

    node_alloc(s);
}

void reset_snake(Snake *s) {
    node_free(s);
    init_snake(s); 
}

void render_snake(SDL_Renderer *renderer, Snake *s) {
    Color c1 = { .r = 167, .g = 65, .b = 198, .a = 255 };
    Color c2 = { .r = 12, .g = 254, .b = 123, .a = 0 };
    Color c = lerp(c1, c2, 0.f);

    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_Rect snake;
    snake.w = CELL;
    snake.h = CELL;

    snake.x = s->x;
    snake.y = s->y;

    SDL_RenderFillRect(renderer, &snake);

    for (int i = 0; i < s->len; ++i) {
        c = lerp(c1, c2, normalize(i+1, 0, s->len+1));
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        snake.x = s->body[i].x;
        snake.y = s->body[i].y;
        SDL_RenderFillRect(renderer, &snake);
    }

}

void update_snake(Snake *s) {
    s->len += 1;
    if (s->len >= s->cap) {
        s->cap *= 2; 
        s->body = realloc(s->body, sizeof(*s->body)*s->cap);
        assert(s->body != NULL);
    }

    swap(s);

}

void move_snake(Snake *s) {
    swap(s);

    switch (s->dir) {
        case UP:    s->y -= CELL; break;
        case DOWN:  s->y += CELL; break;
        case LEFT:  s->x -= CELL; break;
        case RIGHT: s->x += CELL; break;
        default: assert(0 && "Not reachable\n");
    }

    if (s->x < 0) s->x = SCREEN_WIDTH;
    if (s->x > SCREEN_WIDTH) s->x = 0;
    if (s->y < 0) s->y = SCREEN_HEIGHT;
    if (s->y > SCREEN_HEIGHT) s->y = 0;

}

bool has_hit_itself(Snake *s) {
    bool res = false;

    for (int i = 0; i < s->len; i++) {
        if (s->x == s->body[i].x && s->y == s->body[i].y) {
            res = true;
        }
    }

    return res;
}

void render_apple(SDL_Renderer *renderer, SDL_Rect *a) {
    a->w = CELL;
    a->h = CELL;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, a);
}

void generate_apple(SDL_Rect *a, Snake *s) {
    bool is_valid;
    do {
        is_valid = true;
        a->x = (rand()%SCREEN_WIDTH/CELL)*CELL;
        a->y = (rand()%SCREEN_HEIGHT/CELL)*CELL;
        for (int i = 0; i < s->len; ++i) {
            if (a->x == s->body[i].x && a->y == s->body[i].y) is_valid = false;
        }
    } while (!is_valid);
}

void render_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0x18, 0x18, 0x18, 0xFF);
    SDL_Rect grid;

    grid.y = 0;
    grid.w = 1;
    grid.h = SCREEN_HEIGHT;
    for (grid.x = 0; grid.x < SCREEN_WIDTH; grid.x += CELL) {
        SDL_RenderDrawRect(renderer, &grid);
    }

    grid.x = 0;
    grid.w = SCREEN_WIDTH;
    grid.h = 1;
    for (grid.y = 0; grid.y < SCREEN_HEIGHT; grid.y += CELL) {
        SDL_RenderDrawRect(renderer, &grid);
    }
}

int main(void) {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Snake",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return -1;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return -1;

    Snake snake = {0};
    init_snake(&snake); 

    SDL_Rect apple;
    generate_apple(&apple, &snake);

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
            SDL_RenderClear(renderer);

            move_snake(&snake);
            if (has_hit_itself(&snake)) reset_snake(&snake);

            if (snake.x == apple.x && snake.y == apple.y) {
                update_snake(&snake);
                generate_apple(&apple, &snake);
            }

            render_snake(renderer, &snake);
            render_apple(renderer, &apple);
            render_grid(renderer);

            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderPresent(renderer);
        }
        
        SDL_Delay(120);
    }

    node_free(&snake);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

