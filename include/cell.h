#ifndef CELL_H
#define CELL_H

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// MacOS
#if defined(__APPLE__)
    #include <SDL_image.h>
#else
// Linux
    #include <SDL2/SDL_image.h>
#endif

typedef struct Ray Ray;
typedef struct Cell Cell;

#include "game.h"
#include "neuralNetwork.h"
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
    double outputs[2];

    SDL_FPoint position;
    SDL_FPoint positionInit;
    float angle;
    float angleVelocity;
    float angleVelocityMax;

    float speed;
    float speedMax;
    float velocity;

    bool goingUp;
    bool goingDown;
    bool goingLeft;
    bool goingRight;

    int radius;
    SDL_FRect hitbox;
    SDL_Texture *sprite;
};


Cell *Cell_create(SDL_Texture *sprite, int x, int y, bool isAI);
void Cell_update(Cell *cell, Map *map);
void Cell_mutate(Cell *cell, float mutationRate, float mutationProbability, float topologyMutationProbability);
void Cell_GiveBirth(Cell *cell, Map *map);
void Cell_render(Cell *cell, SDL_Renderer *renderer, bool renderRays, bool isSelected);
void Cell_reset(Cell *cell);
void Cell_destroy(Cell *cell);

// Collisions
bool check_rect_collision(Cell *cell, SDL_FRect *hitbox);
float check_ray_collision(Cell *cell, SDL_FRect *hitbox, int rayIndex);

// Sprite loading and management functions
bool load_all_cell_sprites(SDL_Renderer *renderer);
void free_cell_sprites(void);

#endif // CELL_H
