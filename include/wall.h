#ifndef WALL_H
#define WALL_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

typedef struct Wall Wall;

#include "utils.h"


struct Wall
{
    SDL_FRect rect;
    SDL_Color color;
};

Wall *Wall_init(float x, float y, float width, float height);
void Wall_render(Wall *wall, SDL_Renderer *renderer);
void Wall_destroy(Wall *wall);

#endif // WALL_H
