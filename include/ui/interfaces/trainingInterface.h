#ifndef TRAINING_INTERFACE_H
#define TRAINING_INTERFACE_H

#include <SDL2/SDL.h>
#include "../../core/game.h"

/**
 * Render the training dashboard
 * @param renderer SDL renderer
 * @param map Game map
 */
void TrainingInterface_RenderDashboard(SDL_Renderer *renderer, Map *map);

/**
 * Render training metrics panel
 * @param renderer SDL renderer
 * @param map Game map
 * @param x X position
 * @param y Y position
 * @param updateData Whether to update cached data
 */
void TrainingInterface_RenderMetrics(SDL_Renderer *renderer, Map *map, int x, int y, bool updateData);

/**
 * Render training graphs panel
 * @param renderer SDL renderer
 * @param map Game map
 * @param x X position
 * @param y Y position
 */
void TrainingInterface_RenderGraphs(SDL_Renderer *renderer, Map *map, int x, int y);

#endif // TRAINING_INTERFACE_H
