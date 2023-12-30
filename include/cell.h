#ifndef CELL_H
#define CELL_H

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL2/SDL_image.h>

typedef struct Ray Ray;
typedef struct Cell Cell;

#include "game.h"
#include "NeuralNetwork.h"
#include "collisions.h"
#include "wall.h"
#include "utils.h"


struct Ray
{
    float angle;
    float distance;
    float distanceMax;
};
struct Cell
{
    bool isAlive;
    int score; // eatenFood
    Ray rays[7];
    Ray raysWall[7];
    int generation;

    int health;
    int healthInit;
    int healthMax;
    int frame;
    int framePerHealth;
    int birthCostMax;

    bool isAI;
    NeuralNetwork *nn;
    double inputs[15];
    double outputs[4];

    SDL_FPoint position;
    SDL_FPoint positionInit;
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

    int radius;
    SDL_Color color;
    SDL_Texture *sprite;
};


Cell *Cell_init(SDL_Texture *texture, int x, int y, bool isAI);
void Cell_update(Cell *cell, Map *map);
void Cell_mutate(Cell *cell, Cell *parent, float mutationRate, float mutationProbability);
void Cell_GiveBirth(Cell *cell, Map *map);
void Cell_render(Cell *cell, SDL_Renderer *renderer, bool renderRays, bool isSelected);
void Cell_reset(Cell *cell);
void Cell_destroy(Cell *cell);

#endif // CELL_H
