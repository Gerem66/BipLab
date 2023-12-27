#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL2_framerate.h>
#include <SDL2_gfxPrimitives.h>

typedef struct Map Map;

#include "cell.h"
#include "wall.h"


struct Map
{
    Cell *cells[10];
    Wall *walls[4];
};


bool Game_start(SDL_Renderer *renderer, int w, int h);
void Game_reset(Map *map);

#endif // GAME_H
