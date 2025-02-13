#ifndef FOOD_H
#define FOOD_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

typedef struct Food Food;

#include "game.h"
#include "utils.h"


struct Food
{
    int value;
    SDL_FRect rect;
    SDL_Color color;
};

Food *Food_init(float x, float y);
void Food_reset(Food *food, Map *map);
void Food_render(Food *wall, SDL_Renderer *renderer, bool renderTexts);
void Food_destroy(Food *wall);

#endif // FOOD_H
