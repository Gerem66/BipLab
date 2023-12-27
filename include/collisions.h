#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#include "cell.h"
#include "utils.h"


bool check_rect_collision(Cell *cell, SDL_Rect *hitbox);

#endif // COLLISIONS_H
