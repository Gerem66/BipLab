#ifndef CELL_H
#define CELL_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

typedef struct Cell Cell;

#include "game.h"
#include "collisions.h"
#include "wall.h"
#include "utils.h"


struct Cell
{
    bool isAlive;
    int score;

    SDL_Point position;
    SDL_Point positionInit;
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


Cell *Cell_init(int x, int y, int radius);
void Cell_update(Cell *cell, Map *map);
void Cell_render(Cell *cell, SDL_Renderer *renderer);
void Cell_reset(Cell *cell);
void Cell_destroy(Cell *cell);

#endif // CELL_H
