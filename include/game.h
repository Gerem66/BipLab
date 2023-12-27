#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL2_framerate.h>
#include <SDL2_gfxPrimitives.h>

#include "cell.h"


bool Game_start(SDL_Renderer *renderer, int w, int h);

#endif // GAME_H
