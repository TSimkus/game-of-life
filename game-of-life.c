#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include <stdlib.h>

#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"


#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 900

#define ROW_SIZE 80
#define COL_SIZE 80

#define DIRECTIONS_SIZE 8
#define SPEED_RANGE_SIZE 5

int directions[DIRECTIONS_SIZE][2] = {
    {0, 1},   // TOP
    {1, 1},   // TOP RIGHT
    {1, 0},   // RIGHT
    {1, -1},  // BOTTOM RIGHT
    {0, -1},  // BOTTOM
    {-1, -1}, // BOTTOM LEFT
    {-1, 0},  // LEFT
    {-1, 1},  // TOP LEFT
};

typedef struct {
    bool alive;
} Cell;

typedef Cell Cells[ROW_SIZE][COL_SIZE];

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Cells cells;
} AppState;

void draw_cells(AppState *appstate)
{
    SDL_FRect rect;
    float cell_width = (float)WINDOW_WIDTH / ROW_SIZE;
    float cell_height = (float)WINDOW_HEIGHT / COL_SIZE;

    for (int col = 0; col < COL_SIZE; col++) {
        for (int row = 0; row < ROW_SIZE; row++) {
            rect.x = cell_width * row;
            rect.y = cell_height * col;
            rect.w = cell_width;
            rect.h = cell_height;

            if (appstate->cells[row][col].alive == true) {
                SDL_SetRenderDrawColor(appstate->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(appstate->renderer, &rect);
            } else {
                SDL_SetRenderDrawColor(appstate->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
            }

            SDL_RenderRect(appstate->renderer, &rect);
        }
    }
}

void init_alive_cells(AppState *appstate)
{
    for (int col = 0; col < COL_SIZE; col++) {
        for (int row = 0; row < ROW_SIZE; row++) {
            bool alive = rand() % 2;
            appstate->cells[row][col].alive = alive;
        }
    }
}

bool is_out_of_bounds(int row, int col)
{
    return (row < 0 || row >= ROW_SIZE || col < 0 || col >= COL_SIZE);
}

int count_cell_active_neighbours(AppState *appstate, int row, int col)
{
    int alive_neighbours = 0;

    for (int key = 0; key < DIRECTIONS_SIZE; key++) {
        int new_row = row + directions[key][0];
        int new_col = col + directions[key][1];

        if (is_out_of_bounds(new_row, new_col)) {
            continue;
        }

        if (appstate->cells[new_row][new_col].alive == true) {
            alive_neighbours++;
        }
    }

    return alive_neighbours;
}

void update_active_cells(AppState *appstate)
{
    Cells cells = {0};

    for (int col = 0; col < COL_SIZE; col++) {
        for (int row = 0; row < ROW_SIZE; row++) {

            int alive_neighbours = count_cell_active_neighbours(appstate, row, col);

            // handle cell state
            if (appstate->cells[row][col].alive == true && (alive_neighbours == 2 || alive_neighbours == 3)) {
                cells[row][col].alive = true;
            } else if (appstate->cells[row][col].alive == false && alive_neighbours == 3) {
                cells[row][col].alive = true;
            }
        }
    }

    SDL_memcpy(appstate->cells, cells, sizeof(Cells));
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    AppState *as = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!as) {
        SDL_Log("Game State is not initialized");

        return SDL_APP_FAILURE;
    }

    init_alive_cells(as);

    *appstate = as;

    if (!SDL_CreateWindowAndRenderer("Game of life", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer)) {
        SDL_Log("Window and Renderer creation failed. Reason: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *as = (AppState *)appstate;

    SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(as->renderer);

    draw_cells(as);

    SDL_RenderPresent(as->renderer);

    update_active_cells(as);

    SDL_Delay(100);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (appstate != NULL) {
        AppState *as = (AppState *)appstate;
        SDL_DestroyRenderer(as->renderer);
        SDL_DestroyWindow(as->window);
        SDL_free(as);
    }
}
