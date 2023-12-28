#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#include "cell.h"
#include "utils.h"


bool check_rect_collision(Cell *cell, SDL_FRect *hitbox);
float check_ray_collision(Cell *cell, SDL_FRect *hitbox, int rayIndex);

#endif // COLLISIONS_H
