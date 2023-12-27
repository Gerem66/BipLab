#ifndef CELL_H
#define CELL_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#include "utils.h"


struct Cell
{
    SDL_Point position;
    float angle;
    float angleVelocity;
    float angleVelocityMax;

    float speed;
    float speedMin;
    float speedMax;
    float velocity;

    bool goingUp;
    bool goingDown;
    bool goingLeft;
    bool goingRight;

    //int health;
    //int healthMax;

    int radius;
    SDL_Color color;
};
typedef struct Cell Cell;


Cell *Cell_init(int x, int y, int radius);
void Cell_update(Cell *cell);
void Cell_render(Cell *cell, SDL_Renderer *renderer);
void Cell_destroy(Cell *cell);

#endif // CELL_H
