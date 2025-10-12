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

#include "../core/game.h"
#include "../ai/neuralNetwork.h"
#include "wall.h"
#include "../core/utils.h"


typedef enum {
    RAY_OBJECT_NONE = 0,
    RAY_OBJECT_FOOD = 1,
    RAY_OBJECT_CELL = 2,
    RAY_OBJECT_WALL = 3,
    RAY_OBJECT_COUNT  // Automatic count - always keep last
} RayObjectType;

typedef struct {
    RayObjectType type;
    float distance;
    float value;
} RayHit;

struct Ray
{
    float angle;
    float distance;
    float distanceMax;
    RayHit hit;
};
struct Cell
{
    bool isAlive;
    int score; // eatenFood
    Ray rays[7];
    int generation;

    int health;
    int healthInit;
    int healthMax;
    int frame;
    int birthCostMax;

    bool isAI;
    NeuralNetwork *nn;
    double inputs[30]; // 1 health + 1 can_reproduce + 7 rays * 4 features
    double outputs[3]; // acceleration + rotation + reproduction

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


Cell *Cell_create(int x, int y, bool isAI);
void Cell_update(Cell *cell, Map *map);
void Cell_mutate(Cell *cell, float mutationRate, float mutationProbability);
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
