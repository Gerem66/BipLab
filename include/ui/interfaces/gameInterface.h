#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <SDL2/SDL.h>
#include "../../core/game.h"

/**
 * Main game rendering function
 * @param renderer SDL renderer
 * @param map Game map
 */
void GameInterface_Render(SDL_Renderer *renderer, Map *map);

/**
 * Render text overlay information
 * @param map Game map
 * @param color Text color
 */
void GameInterface_RenderText(Map *map, SDL_Color color);

/**
 * Render zoom bar
 * @param map Game map
 * @param color Bar color
 * @param x X position
 * @param y Y position
 */
void GameInterface_RenderZoomBar(Map *map, SDL_Color color, int x, int y);

#endif // GAME_INTERFACE_H
