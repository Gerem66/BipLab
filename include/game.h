#ifndef GAME_H
#define GAME_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL2_framerate.h>
#include <SDL2_gfxPrimitives.h>

typedef struct Map Map;

#include "cell.h"
#include "wall.h"

#define CELL_COUNT 100


struct Map
{
    time_t startTime;
    Cell *cells[CELL_COUNT];
    Wall *walls[4];
    int generation;
    int frames;
    int maxScore;
    int maxAverageScore;
    bool isRunning;
    bool verticalSync;
    bool renderEnabled;
};


bool Game_start(SDL_Renderer *renderer, int w, int h);
void Game_reset(Map *map);
void Game_render(SDL_Renderer *renderer, Map *map);
void Render_Text(SDL_Renderer *renderer, Map *map, SDL_Color color);

#endif // GAME_H
